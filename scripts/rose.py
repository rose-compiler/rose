########################################################################
# Imports
########################################################################

import cPickle
import errno
import os
import qm.common
import qm.executable
import qm.fields
import qm.test.base
from   qm.test.test import Test
from   qm.test.result import Result
import string
import sys
import types

########################################################################
# Classes
########################################################################

class RoseObjectTestBase(Test):
    """Check a program's output and exit code.

    An 'RoseTestBase' runs a program and compares its standard output,
    standard error, and exit code with expected values.  The program
    may be provided with command-line arguments and/or standard
    input.

    The test passes if the standard output, standard error, and
    exit code are identical to the expected values."""

    arguments = [
        qm.fields.TextField(
            name="stdin",
            title="Standard Input",
            verbatim="true",
            multiline="true",
            description="""The contents of the standard input stream.

            If this field is left blank, the standard input stream will
            contain no data."""
            ),

        qm.fields.SetField(qm.fields.TextField(
            name="environment",
            title="Environment",
            description="""Additional environment variables.

            By default, QMTest runs tests with the same environment that
            QMTest is running in.  If you run tests in parallel, or
            using a remote machine, the environment variables available
            will be dependent on the context in which the remote test
            is executing.

            You may add variables to the environment.  Each entry must
            be of the form 'VAR=VAL'.  The program will be run in an
            environment where the environment variable 'VAR' has the
            value 'VAL'.  If 'VAR' already had a value in the
            environment, it will be replaced with 'VAL'.

            In addition, QMTest automatically adds an environment
            variable corresponding to each context property.  The name
            of the environment variable is the name of the context
            property, prefixed with 'QMV_'.  For example, if the value
            of the context property named 'target' is available in the
            environment variable 'QMV_target'.  Any dots in the context
            key are replaced by a double-underscore; e.g.,
            "CompilerTable.c_path" will become
            "QMV_CompilerTable__c_path".""" )),
        
        qm.fields.IntegerField(
            name="exit_code",
            title="Exit Code",
            description="""The expected exit code.

            Most programs use a zero exit code to indicate success and a
            non-zero exit code to indicate failure."""
            ),

        qm.fields.TextField(
            name="stdout",
            title="Standard Output",
            verbatim="true",
            multiline="true",
            description="""The expected contents of the standard output stream.

            If the output written by the program does not match this
            value, the test will fail."""
            ),
        
        qm.fields.TextField(
            name="stderr",
            title="Standard Error",
            verbatim="true",
            multiline="true",
            description="""The expected contents of the standard error stream.

            If the output written by the program does not match this
            value, the test will fail."""
            ),

        qm.fields.IntegerField(
            name="timeout",
            title="Timeout",
            description="""The number of seconds the child program will run.

            If this field is non-negative, it indicates the number of
            seconds the child program will be permitted to run.  If this
            field is not present, or negative, the child program will be
            permitted to run for ever.""",
            default_value = -1,
            ),
        ]


    def MakeEnvironment(self, context):
        """Construct the environment for executing the target program."""

        # Start with any environment variables that are already present
        # in the environment.
        environment = os.environ.copy()
        # Copy context variables into the environment.
        for key, value in context.items():
            # If the value has unicode type, only transfer
            # it if it can be cast to str.
            if  isinstance(value, unicode):
                try:
                    value = str(value)
                except UnicodeEncodeError:
                    continue
            if  isinstance(value, str):
                name = "QMV_" + key.replace(".", "__")
                environment[name] = value
        # Extract additional environment variable assignments from the
        # 'Environment' field.
        for assignment in self.environment:
            if "=" in assignment:
                # Break the assignment at the first equals sign.
                variable, value = string.split(assignment, "=", 1)
                environment[variable] = value
            else:
                raise ValueError, \
                      qm.error("invalid environment assignment",
                               assignment=assignment)
        return environment


    def ValidateOutput(self, stdout, stderr, result, objArg):
        """Validate the output of the program. Check for the presence of ROSE
	   compiled object file specified in arguments.

        'stdout' -- A string containing the data written to the standard output
        stream.

        'stderr' -- A string containing the data written to the standard error
        stream.

        'result' -- A 'Result' object. It may be used to annotate
        the outcome according to the content of stderr.

        returns -- A list of strings giving causes of failure."""

        causes = []

	# parse out the obj argument from source file to object file extension
	object = objArg.split("/")
	object = object[len(object) - 1].split(".")
	object = object[0] + ".o"

	# build and append the object file to the present working path
	pwd = os.getcwd()
	pwd = pwd + "/"
	pwd = pwd + object

	if not os.path.exists( pwd ):
	  causes.append( object + " Not Found!" )
	  result["RoseTest.expected_stdout"] = result.Quote(self.stdout)

        return causes
	
	# old code that worked with a modified translator script; not needed!
	# search for presence of object file, if not found test failed
	# if stdout.find(object) == -1:
	#  causes.append(object + " Not Found!")
	#  result["RoseTest.expected_stdout"] = result.Quote(self.stdout)

    def RunProgram(self, program, arguments, context, result):
        """Run the 'program'.

        'program' -- The path to the program to run.

        'arguments' -- A list of the arguments to the program.  This
        list must contain a first argument corresponding to 'argv[0]'.

        'context' -- A 'Context' giving run-time parameters to the
        test.

        'result' -- A 'Result' object.  The outcome will be
        'Result.PASS' when this method is called.  The 'result' may be
        modified by this method to indicate outcomes other than
        'Result.PASS' or to add annotations."""

        # Construct the environment.
        environment = self.MakeEnvironment(context)
        # Create the executable.
        if self.timeout >= 0:
            timeout = self.timeout
        else:
            # If no timeout was specified, we sill run this process in a
            # separate process group and kill the entire process group
            # when the child is done executing.  That means that
            # orphaned child processes created by the test will be
            # cleaned up.
            timeout = -2
        e = qm.executable.Filter(self.stdin, timeout)
        # Run it.
        exit_status = e.Run(arguments, environment, path = program)

        # If the process terminated normally, check the outputs.
        if sys.platform == "win32" or os.WIFEXITED(exit_status):
            # There are no causes of failure yet.
            causes = []
            # The target program terminated normally.  Extract the
            # exit code, if this test checks it.
            if self.exit_code is None:
                exit_code = None
            elif sys.platform == "win32":
                exit_code = exit_status
            else:
                exit_code = os.WEXITSTATUS(exit_status)
            # Get the output generated by the program.
            stdout = e.stdout
            stderr = e.stderr
            # Record the results.
            result["RoseTest.exit_code"] = str(exit_code)
            result["RoseTest.stdout"] = result.Quote(stdout)
            result["RoseTest.stderr"] = result.Quote(stderr)
            # Check to see if the exit code matches.
            if exit_code != self.exit_code:
                causes.append("exit_code")
                result["RoseTest.expected_exit_code"] = str(self.exit_code)

            # Validate the output.
            causes += self.ValidateOutput(stdout, stderr, result, arguments[len(arguments)-1])

            # If anything went wrong, the test failed.
            if causes:
                result.Fail("Unexpected %s." % string.join(causes, ", ")) 

        elif os.WIFSIGNALED(exit_status):
            # The target program terminated with a signal.  Construe
            # that as a test failure.
            signal_number = str(os.WTERMSIG(exit_status))
            result.Fail("Program terminated by signal.")
            result["RoseTest.signal_number"] = signal_number

	# START MODIFICATION GMY 7/26/2006

            # Get the output generated by the program.
            stdout = e.stdout
            stderr = e.stderr

	    result["RoseTest.stdout"] = stdout
	    result["RoseTest.stderr"] = stderr

	# END MODIFICATION GMY 7/26/2006

        elif os.WIFSTOPPED(exit_status):
            # The target program was stopped.  Construe that as a
            # test failure.
            signal_number = str(os.WSTOPSIG(exit_status))
            result.Fail("Program stopped by signal.")
            result["RoseTest.signal_number"] = signal_number
        else:
            # The target program terminated abnormally in some other
            # manner.  (This shouldn't normally happen...)
            result.Fail("Program did not terminate normally.")


class RoseTest(RoseObjectTestBase):
    """Check a program's output and exit code.

    An 'RoseTest' runs a program by using the 'exec' system call."""

    arguments = [
        qm.fields.TextField(
            name="rose",
            title="ROSE PATH",
            not_empty_text=1,
            description="""The path to the ROSE translator.
	    """
            ),
        
        qm.fields.SetField(qm.fields.TextField(
            name="arguments",
            title="Argument List",
            description="""The command-line arguments.

            If this field is left blank, the program is run without any
            arguments.

            An implicit 0th argument (the path to the program) is added
            automatically."""
            ))]

    _allow_arg_names_matching_class_vars = 1


    def Run(self, context, result):
        """Run the test.

        'context' -- A 'Context' giving run-time parameters to the
        test.

        'result' -- A 'Result' object.  The outcome will be
        'Result.PASS' when this method is called.  The 'result' may be
        modified by this method to indicate outcomes other than
        'Result.PASS' or to add annotations."""

        # Was the program not specified?
        if string.strip(self.rose) == "":
            result.Fail("No path to ROSE specified.")
            return

        self.RunProgram(self.rose, 
                        [ self.rose ] + self.arguments,
                        context, result)
