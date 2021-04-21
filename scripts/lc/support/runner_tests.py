#!/usr/bin/env python
"""Runs unit tests on support.runner.
"""
import unittest
from local_logging import Logger
from runner import Runner


class TestCase1ExecOrLog(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.logger = Logger("support.runner_tests.TestCase1ExecOrLog")
        self.runner = Runner()
        self.command = "print('foo')"
        self.bad_command = "print(nonExistentVar)"

    def tearDown(self):
        self.runner = None
        unittest.TestCase.tearDown(self)

    def test01_EffortOnly(self):
        self.logger.info("Exercising effortOnly=True.  Command should not execute.")
        self.runner.setEffortOnly(True)
        self.runner.execOrLog(self.command)

    def test02_DoIt(self):
        self.logger.info("Exercising effortOnly=False.  Command should execute.")
        self.runner.setEffortOnly(False)
        self.runner.execOrLog(self.command)

    def test03_HandleExcep(self):
        self.logger.info("Exercising runAndHandle.  Exception should be logged and not reraised.")
        self.runner.execOrLog(self.bad_command)

    def test04_ReraiseExcep(self):
        self.logger.info("Exercising runAndHandle.  Exception should be logged and reraised.")
        self.assertRaises(
            Runner.Failed,
            self.runner.execOrLog, self.bad_command, doReraise=True)


class TestCase2NameSpace(unittest.TestCase):
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.logger = Logger("support.runner_tests.TestCase2NameSpace")
        self.runner = Runner()
        self.bad_command = "atexit.register(abs, 0)"

    def tearDown(self):
        self.runner = None
        unittest.TestCase.tearDown(self)

    def test03_ReraiseExcep(self):
        self.logger.info("Exercising operation not in namespace.  Exception should be logged and raised.")
        self.assertRaises(
            Runner.Failed,
            self.runner.execOrLog, self.bad_command, doReraise=True)


def square(parm_in):
    print ("parm_in is " + str(parm_in))
    return parm_in * parm_in


class TestCase3Eval(unittest.TestCase):
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.logger = Logger("support.runner_tests.TestCase3Eval")
        self.runner = Runner()
        self.expression = "square(parm)"
        self.bad_expression = "nonExistentVar"

    def tearDown(self):
        self.runner = None
        unittest.TestCase.tearDown(self)

    def test01_EffortOnly(self):
        self.logger.info("Exercising effortOnly=True.  Command should not execute.")
        self.runner.setEffortOnly(True)
        result = self.runner.evalOrLog(self.expression, globals(), locals())

    def test02_DoIt(self):
        self.logger.info("Exercising effortOnly=False.  Command should execute.")
        self.runner.setEffortOnly(False)
        parm = 3
        result = self.runner.evalOrLog(self.expression, globals(), locals())
        self.assertEquals(result, 9)

    # No no_reraise test here because there is no no_reraise option in evalOrLog.

    def test04_ReraiseExcep(self):
        self.logger.info("Exercising evalAndHandle.  Exception should be logged and reraised.")
        self.assertRaises(
            Runner.Failed,
            self.runner.evalOrLog, self.bad_expression, globals(), locals())


class TestCase4Popen(unittest.TestCase):
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.logger = Logger("support.runner_tests.TestCase4Popen")
        self.runner = Runner()
        self.dir = "/"
        self.badDir = "/nosuchdir"
        self.popenArgs = ["uname", "-a"]
        self.badPopenArgs = ["nosuchprog", ]

    def tearDown(self):
        self.runner = None
        unittest.TestCase.tearDown(self)

    def test01_EffortOnly(self):
        self.logger.info("Exercising effortOnly=True.  Command should not execute.")
        self.runner.setEffortOnly(True)
        (output, errors) = self.runner.popenOrLog(self.popenArgs, self.dir)
        self.assertTrue(output == "")
        self.assertTrue(errors == "")

    def test02_DoIt(self):
        self.logger.info("Exercising effortOnly=False.  Command should execute.")
        self.runner.setEffortOnly(False)
        (output, errors) = self.runner.popenOrLog(self.popenArgs, self.dir)
        self.logger.info("output:")
        print (output)
        self.logger.info("errors:")
        print (errors)
        self.assertTrue(errors == "")
        self.assertTrue("SunOS" in output or "Linux" in output)

    # No no_reraise test here because there is no no_reraise option in popenOrLog.

    def test04_ReraiseExcep(self):
        self.logger.info("Exercising popenAndHandle.  Exception should be logged and reraised.")
        self.assertRaises(
            Runner.Failed,
            self.runner.popenOrLog, self.badPopenArgs, self.dir)

    def test05_ReraiseExcep(self):
        self.logger.info("Exercising popenAndHandle.  Exception should be logged and reraised.")
        self.assertRaises(
            Runner.Failed,
            self.runner.popenOrLog, self.popenArgs, self.badDir)

class TestCase5Call(unittest.TestCase):
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.logger = Logger("support.runner_tests.TestCase5Call")
        self.runner = Runner()
        self.dir = "/"
        self.badDir = "/nosuchdir"
        self.args = ["uname", "-a"]
        self.badArgs = ["nosuchprog", ]

    def tearDown(self):
        self.runner = None
        unittest.TestCase.tearDown(self)

    def test01_EffortOnly(self):
        self.logger.info("Exercising effortOnly=True.  Command should not execute.")
        self.runner.setEffortOnly(True)
        self.runner.callOrLog(self.args, self.dir)

    def test02_DoIt(self):
        self.logger.info("Exercising effortOnly=False.  Command should execute.")
        self.runner.setEffortOnly(False)
        self.runner.callOrLog(self.args, self.dir)

    # No no_reraise test here because there is no no_reraise option in callOrLog.

    def test04_ReraiseExcep(self):
        self.logger.info("Exercising callAndHandle.  Exception should be logged and reraised.")
        self.assertRaises(
            Runner.Failed,
            self.runner.callOrLog, self.badArgs, self.dir)

    def test05_ReraiseExcep(self):
        self.logger.info("Exercising callAndHandle.  Exception should be logged and reraised.")
        self.assertRaises(
            Runner.Failed,
            self.runner.callOrLog, self.args, self.badDir)


if __name__ == "__main__":
    unittest.main()
