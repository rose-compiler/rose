"""Contains the Runner class, which supports effort-only behavior by logging a 
command instead of running it.
"""
import os
import subprocess

from local_logging import Logger, LineLogger, FunctionLogger


def _exec_info(command, globals, locals):
    return "exec %s" % \
           (str(command),)
# return "exec %s in\\\n%s,\\\n%s" % \
#        (str(command), str(globals), str(locals))

def _eval_info(source, globals, locals):
    return "eval(%s)" % \
           (str(source),)
# return "eval(source=%s,\nglobals=%s,\nlocals=%s)" % \
#        (str(source), str(globals), str(locals))

def _popen_info(args, dir):
    return "subprocess.Popen(\nargs=%s,\ndir=%s)" % \
           (str(args), str(dir))
# return "subprocess.Popen(\nargs=%s,\nenv=%s,\ndir=%s)" % \
#        (str(args), str(os.environ), str(dir))

def _check_call_info(args, dir):
    return "subprocess.check_call(\nargs=%s,\ndir=%s)" % \
           (str(args), str(dir))
# return "subprocess.check_call(\nargs=%s,\nenv=%s,\ndir=%s)" % \
#        (str(args), str(os.environ), str(dir))

def _exception_info(e):
    return '%s args:%s' % (type(e), str(e))

def _exception_message(e, info):
    return 'EXCEPTION "%s" raised while running "%s"' % (_exception_info(e), info)

class Runner():
    class Failed(Exception):
        pass

    def __init__(self, effortOnly=False):
        self._effortOnly = effortOnly
        self._logger = Logger(
            name='support.runner.Runner',
            level=Logger.DEBUG)

    def setEffortOnly(self, effortOnly):
        self._effortOnly = effortOnly

    # exec ---------------------------------------------------------------------

    def execOrLog(self, command, globals=None, locals=None, doReraise=False):
        info_message = "\n%s" % _exec_info(command, globals, locals)
        if self._effortOnly:
            self._logger.info("Would do:%s" % info_message)
        else:
            self._logger.info("Doing:%s" % info_message)
            self.tryExec(command, globals, locals, doReraise)

    def tryExec(self, command, globals=None, locals=None, doReraise=False):
        try:
            exec command in globals, locals
        except Exception as e:
            message = _exception_message(e, _exec_info(command, globals, locals))
            self._logger._logger.exception(message)
            if doReraise:
                self._logger.error("Raising Runner.Failed")
                raise self.Failed(message)
            else:
                self._logger.info("Continuing...")

    # eval ---------------------------------------------------------------------

    def evalOrLog(self, expression, globals=None, locals=None):
        info_message = "\n%s" % _eval_info(expression, globals, locals)
        if self._effortOnly:
            self._logger.info("Would do:%s" % info_message)
        else:
            self._logger.info("Doing:%s" % info_message)
            return self.tryEval(expression, globals, locals)

    def tryEval(self, expression, globals=None, locals=None):
        """Evaluates the expression in command and returns the result.  Always
        reraises any exceptions.
        """
        try:
            return eval(expression, globals, locals)
        except Exception as e:
            message = _exception_message(e, _eval_info(expression, globals, locals))
            self._logger._logger.exception(message)
            self._logger.error("Raising Runner.Failed")
            raise self.Failed(message)

    # Popen --------------------------------------------------------------------

    # (callOrLog, below, waits for the child to finish, and pipes output instead
    # of teturning it.  Recommended instead of popenOrLog.)

    def popenOrLog(self, callArgs, dir=None):
        info_message = "\n%s" % _popen_info(callArgs, dir)
        if self._effortOnly:
            self._logger.info("Would do:%s" % info_message)
            # Caller expects a tuple:
            return ("", "")
        else:
            self._logger.info("Doing:%s" % info_message)
            return self.tryPopen(callArgs, dir)

    def tryPopen(self, callArgs, dir=None):
        """Issue the command associated with the given Popen arguments list.
        Returns the results of the command in a (output, errors) tuple once the
        command finishes. Raises Runner.Failed if check_call raises OSError.
        """
        try:
            p1 = subprocess.Popen(
                args=callArgs,
                env=os.environ,
                cwd=dir,
                # Need PIPE here or else output, below will be None:
                stdout=subprocess.PIPE,
                # Need PIPE here or else errors, below will be None
                stderr=subprocess.PIPE)
            (output, errors) = p1.communicate()
        except OSError as e:
            message = _exception_message(e, _popen_info(callArgs, dir))
            self._logger._logger.exception(message)
            self._logger.error("Raising Runner.Failed")
            raise self.Failed(message)
        return output, errors

    # check_call ---------------------------------------------------------------

    def callOrLog(self, callArgs, dir=None):
        info_message = "\n%s" % _check_call_info(callArgs, dir)
        if self._effortOnly:
            self._logger.info("Would do:%s" % info_message)
        else:
            self._logger.info("Doing:%s" % info_message)
            self.tryCall(callArgs, dir)

    def tryCall(self, callArgs, dir=None):
        """Issues the command in callArgs. Pipes the output to the logger's
        stream.  Returns when command does.  Raises Runner.Failed if check_call
        raises OSError.
        """
        try:
            # Output goes to stdout and stderr:
            subprocess.check_call(
                args=callArgs,
                env=os.environ,
                cwd=dir,
                stdout=self._logger.get_stream(),
                stderr=subprocess.STDOUT
            )
        except subprocess.CalledProcessError as e:
            message = _exception_message(e, _check_call_info(callArgs, dir))
            #self._logger._logger.exception(message)
            self._logger.error("Raising Runner.Failed")
            raise self.Failed(message)

    # Synonyms
    runOrLog = execOrLog

runner = Runner()
