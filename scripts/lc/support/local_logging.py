""" Provides logging behavior.  Declare a Logger object, then make calls.
"""
import functools
import logging
from string_utils import su
import sys


class Logger(object):
    # Define local levels and corresponding prefix strings:
    ERROR = logging.ERROR  # 40
    logging.addLevelName(ERROR, "***")
    PROBLEM = 38
    logging.addLevelName(PROBLEM, "--*")

    WARNING = logging.WARNING  # 30
    logging.addLevelName(WARNING, "!!!")

    SUCCESS = 25
    logging.addLevelName(SUCCESS, "+++")
    PROGRESS = 23
    logging.addLevelName(PROGRESS, "--+")

    LOG = 21
    logging.addLevelName(LOG, "")
    INFO = logging.INFO  # 20
    logging.addLevelName(INFO, "---")
    MORE = 18
    logging.addLevelName(MORE, "...")
    NONDEBUG = MORE

    DEBUG = logging.DEBUG  # 10
    logging.addLevelName(DEBUG, "$$$")

    NOTSET = logging.NOTSET  # 0
    logging.addLevelName(NOTSET, "   ")

    DEFAULT_LEVEL = NONDEBUG

    def _make_format(self, name, show_name, show_date, show_time, show_func, show_line):
        """ Set up output line parts:
        # [date] [time] <level> [(logger name)] [function name] [module:source line] <message>
        """
        format_string = ''
        if show_date or show_time:
            format_string = su.cws(format_string, '%(asctime)s')
            date_format_string = ''
            if show_date:
                date_format_string = su.cws(date_format_string, '%Y-%m-%d')
            if show_time:
                date_format_string = su.cws(date_format_string, '%H:%M:%S')
        else:
            date_format_string = None
        format_string = su.cws(format_string, '%(levelname)s')
        if show_name and name:
            format_string = su.cws(format_string, '(%(name)s)')
        if show_func and show_line:
            format_string = su.cws(format_string, '%(module)s.%(funcName)s:%(lineno)d:')
        else:
            if show_func:
                format_string = su.cws(format_string, '%(module)s.%(funcName)s:')
            if show_line:
                format_string = su.cws(format_string, '%(module)s:%(lineno)d:')
        format_string = su.cws(format_string, '%(message)s')
        return logging.Formatter(format_string, date_format_string)

    def __init__(self,
                 name="default logger",
                 stream=sys.stdout,
                 level=DEFAULT_LEVEL,
                 show_name=True,
                 show_date=True,
                 show_time=True,
                 show_func=False,
                 show_line=False):
        self._logger = logging.getLogger(name)
        # Clear out any old handlers if we get an already-initialized logger:
        self.disable()
        self._stream = stream
        self._handler = logging.StreamHandler(self._stream)
        self._handler.setFormatter(self._make_format(name, show_name, show_date, show_time, show_func, show_line))
        self._logger.setLevel(level)
        self.enable()

    def get_stream(self):
        return self._stream

    def set_debug_on(self):
        self._logger.setLevel(Logger.DEBUG)

    def set_debug_off(self):
        self._logger.setLevel(Logger.NONDEBUG)

    def set_debug(self, on):
        if on:
            self.set_debug_on()
        else:
            self.set_debug_off()

    def disable(self):
        # Clear out any old handlers if we get an already-initialized logger:
        # This is NOT thread-safe:
        for handler in self._logger.handlers:
            self._logger.removeHandler(handler)

    def enable(self):
        self._logger.addHandler(self._handler)

    def log_at_level(self, level,  message):
        self._logger.log(level, message)

    """ operations below are listed in order of precedence - debug is lowest.
    """

    def error(self, message):
        self._logger.log(Logger.ERROR, message)

    def problem(self, message):
        self._logger.log(Logger.PROBLEM, message)

    def warning(self, message):
        self._logger.log(Logger.WARNING, message)

    def success(self, message):
        self._logger.log(Logger.SUCCESS, message)

    def progress(self, message):
        self._logger.log(Logger.PROGRESS, message)

    def log(self, message):
        self._logger.log(Logger.LOG, message)

    def info(self, message):
        self._logger.log(Logger.INFO, message)

    def more(self, message):
        self._logger.log(Logger.MORE, message)

    def debug(self, message):
        self._logger.log(Logger.DEBUG, message)


class LineLogger(Logger):
    """This is just a Logger with show_line turned on.  However, you
    must call it like this to get the right line number: 
    lineLogger=LineLogger(...)
    lineLogger.logger.info("msg")
    lineLogger.logger.debug("msg")
    """

    def __init__(self):
        Logger.__init__(
            self,
            name='_lineLogger',
            level=logging.DEBUG,
            show_name=False,
            show_date=False,
            show_time=False,
            show_func=False,
            show_line=True)


class FunctionLogger(Logger):
    """This is just a Logger with show_func turned on.  However, you
    must call it like this to get the right function name: 
    funcLogger=FunctionLogger(...)
    funcLogger.logger.info("msg")
    funcLogger.logger.debug("msg")
    """

    def __init__(self):
        Logger.__init__(
            self,
            name='_functionLogger',
            level=logging.DEBUG,
            show_name=False,
            show_date=False,
            show_time=False,
            show_func=True,
            show_line=False)


class FunctionLineLogger(Logger):
    """This is just a Logger with show_func and show_lineturned on.  However, you
    must call it like this to get the right function name: 
    funcLogger=FunctionLogger(...)
    funcLogger.logger.info("msg")
    funcLogger.logger.debug("msg")
    """

    def __init__(self):
        Logger.__init__(
            self,
            name='_functionLineLogger',
            level=logging.DEBUG,
            show_name=False,
            show_date=False,
            show_time=False,
            show_func=True,
            show_line=True)


class GeneralAutoLog (object):
    """
    Decorator class.  Decorated functions log ENTRY_MESSAGE before the function
    executes, and log EXIT_MESSAGE afterwards,

    Usage:

    class AutoLog (GeneralAutoLog):
    def __init__(self, func_name=None):
        super(AutoLog, self).__init__(
            logger_name='Regression_Tester',
            func_name=func_name)

    @Auto_Log (or @Auto_Log(<logger name>))
    def My_Func():
        whatever...
    """
    ENTRY_MESSAGE = 'BEGIN {}'
    EXIT_MESSAGE  = 'END   {}'

    def __init__(self, logger=None, func_name=None, level=Logger.INFO):
        """ If logger is supplied, uses that logger.  Otherwise, creates a
        private one at the first invocation of the wrapped function.
        """
        self.logger = logger
        self.func_name = func_name
        self.level = level

    def __call__(self, func):
        """
        :param func:
        :return: wrapper function that calls func
        """
        # functools.wraps makes the returned wrapper function have the name,
        # etc of the original function:
        @functools.wraps(func)
        def wrapper(*args, **kwds):
            """ If logger is not set, uses the logger named <func.__module__>.
            Logs the function entry and exit.

            :return: func result
            """
            if self.logger is None:
                self.logger = Logger(func.__module__)
            if self.func_name is None:
                self.func_name = func.__name__
            self.logger.log_at_level(
                    self.level, self.ENTRY_MESSAGE.format(self.func_name))
            f_result = func(*args, **kwds)
            self.logger.log_at_level(
                    self.level, self.EXIT_MESSAGE.format(self.func_name))
            return f_result

        return wrapper

