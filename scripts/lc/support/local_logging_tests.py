#!/usr/bin/env python
"""Runs unit tests on local_logging.
"""
import unittest
from local_logging import Logger, FunctionLogger, LineLogger, FunctionLineLogger
from local_logging import GeneralAutoLog

def test_a_logger(logger):
    print ("Testing levels: error, problem, warning, success, progress, log, info, and more")
    logger.error("error")
    logger.problem("problem")
    logger.warning("warning")
    logger.success("success")
    logger.progress("progress")
    logger.log("log")
    logger.info("info")
    logger.more("more")


def test_raw_logger(logger):
    print ("Testing raw logging levels: critical, error, warn, info, and debug")
    logger._logger.critical("critical")
    logger._logger.error("error")
    logger._logger.warn("warn")
    logger._logger.info("info")
    logger._logger.debug("debug")


class TestCase01Logger(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def test01_Default(self):
        print("Exercising default logging:")
        logger = Logger()
        test_a_logger(logger)
        print

    def test02_Debug(self):
        print("Exercising debug and level changing behavior:")
        logger = Logger()
        print("Debug is off.  calling debug (should print nothing)")
        logger.debug("debug while off")
        print("Debug is on.  calling debug")
        logger.set_debug_on()
        logger.debug("debug while on")
        print

    def test03_ParentChild(self):
        print("Exercising parent and child behavior:")
        parent = Logger("parent")
        child = Logger("parent.child")
        print("Logging to parent.  Should print once.")
        parent.log("Logging to parent.")
        print("Logging to child.  Should print twice.")
        child.log("Logging to child.")
        parent.disable()
        print("Only child is enabled. Logging to child.  Should print once.")
        child.log("Logging to child.")
        parent.enable()
        child.disable()
        print("Only parent is enabled. Logging to child.  Should print once.")
        child.log("Logging to child.")
        print

    def test04_NoPrefixes(self):
        print("Exercising no prefix logging:")
        logger = Logger(
            name='shortLogger',
            level=Logger.DEBUG,
            show_date=False,
            show_time=False,
            show_name=False)
        test_a_logger(logger)
        print

    def test05_LineNumberLogging(self):
        print("Exercising line number logging:")
        logger = LineLogger()
        test_raw_logger(logger)
        print

    def test06_FunctionNameLogging(self):
        print("Exercising function name logging:")
        logger = FunctionLogger()
        test_raw_logger(logger)
        print

    def test07_FunctionNameLineNumberLogging(self):
        print("Exercising function name line number logging:")
        logger = FunctionLineLogger()
        test_raw_logger(logger)
        print

    def test08_MultipleInits(self):
        print("Exercising mutiple initializes on same logger:")
        logger = Logger("multi")
        print("Initialized once.  Logging.  Should print once.")
        logger.log("log messge")
        logger = Logger("multi", show_date=False)
        print("Initialized again, w/o date output.  Logging.  Should print once.")
        logger.log("log messge")
        print


class TestCase02AutoLogger(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def test01_default_logger(self):
        logger_name = 'local_logging_tests'

        @GeneralAutoLog()
        def test_decorator(parm):
            print('Called with "%s"' % parm)

        func_name = test_decorator.__name__

        print(
        "Exercising GeneralAutoLog with default logger.  Logger name defaults to module name")
        print("Should see:")
        print('<date    > <time  > --- (%s) BEGIN %s' % (logger_name, func_name))
        print('Called with "foo"')
        print('<date    > <time  > --- (%s) END   %s' % (logger_name, func_name))
        print('')
        test_decorator('foo')
        print('')

    def test02_predef_logger(self):
        logger_name = 'should_NOT_see_this_name'
        logger = Logger(
            name=logger_name,
            show_name=False)

        @GeneralAutoLog(logger=logger)
        def test_decorator(parm):
            print('Called with "%s"' % parm)

        func_name = test_decorator.__name__

        print("Exercising GeneralAutoLog with passed-in, initialized logger")
        print("This logger has show_name=False")
        print("Should see:")
        print('<date    > <time  > --- BEGIN %s' % func_name)
        print('Called with "foo"')
        print('<date    > <time  > --- END   %s' % func_name)
        print('')
        test_decorator('foo')
        print('')

    def test03_predef_logger(self):
        logger_name = 'debug logger'
        logger = Logger(
            name=logger_name)
        logger.set_debug_on()

        @GeneralAutoLog(logger=logger, level=Logger.DEBUG)
        def test_decorator(parm):
            print('Called with "%s"' % parm)

        func_name = test_decorator.__name__

        print("Exercising GeneralAutoLog debug level")
        print("Should see:")
        print('<date    > <time  > $$$ (%s) BEGIN %s' % (logger_name, func_name))
        print('Called with "foo"')
        print('<date    > <time  > $$$ (%s) END   %s' % (logger_name, func_name))
        print('')
        test_decorator('foo')
        print('')

    def test04_predef_logger_in_func(self):
        logger_name = 'predef_logger_in_func'
        logger = Logger(logger_name)

        def AutoLog():
            return GeneralAutoLog(logger=logger)

        @AutoLog()
        def test_decorator(parm):
            print('Called with "%s"' % parm)

        func_name = test_decorator.__name__

        print("Exercising GeneralAutoLog with logger supplied by helper func")
        print("Should see:")
        print('<date    > <time  > --- (%s) BEGIN %s' % (logger_name, func_name))
        print('Called with "foo"')
        print('<date    > <time  > --- (%s) END   %s' % (logger_name, func_name))
        print('')
        test_decorator('foo')
        print('')

    def test05_subclass_logger(self):
        logger_name = 'subclass_logger'
        logger = Logger(logger_name)

        class AutoLog01(GeneralAutoLog):
            def __init__(self, func_name=None):
                super(AutoLog01, self).__init__(
                    logger=logger,
                    func_name=func_name)

        @AutoLog01()
        def test_decorator(parm):
            print('Called with "%s"' % parm)

        func_name = test_decorator.__name__

        print("Exercising GeneralAutoLog with logger supplied by subclass")
        print("Should see:")
        print('<date    > <time  > --- (%s) BEGIN %s' % (logger_name, func_name))
        print('Called with "foo"')
        print('<date    > <time  > --- (%s) END   %s' % (logger_name, func_name))
        print('')
        test_decorator('foo')
        print('')

    def test06_subclass_logger(self):
        logger_name = 'subclass_debug_logger'
        logger = Logger(logger_name)
        logger.set_debug_on()

        class AutoLog01(GeneralAutoLog):
            def __init__(self, func_name=None):
                super(AutoLog01, self).__init__(
                    logger=logger,
                    func_name=func_name,
                    level=Logger.DEBUG)

        @AutoLog01()
        def test_decorator(parm):
            print('Called with "%s"' % parm)

        func_name = test_decorator.__name__

        print("Exercising GeneralAutoLog with debug logger supplied by subclass")
        print("Should see:")
        print('<date    > <time  > $$$ (%s) BEGIN %s' % (logger_name, func_name))
        print('Called with "foo"')
        print('<date    > <time  > $$$ (%s) END   %s' % (logger_name, func_name))
        print('')
        test_decorator('foo')
        print('')

    def test07_func_name(self):
        logger_name = 'local_logging_tests'
        func_name = 'new_func_name'

        @GeneralAutoLog(func_name=func_name)
        def test_decorator(parm):
            print('Called with "%s"' % parm)

        print("Exercising GeneralAutoLog explicit function name.")
        print("Should see:")
        print('<date    > <time  > --- (%s) BEGIN %s' % (logger_name,func_name))
        print('Called with "foo"')
        print('<date    > <time  > --- (%s) END   %s' % (logger_name, func_name))
        print('')
        test_decorator('foo')
        print('')

    def test08_class_name(self):
        logger_name = 'local_logging_tests'
        func_name = 'MyClass'

        class MyClass(object):

            @GeneralAutoLog(func_name=func_name)
            def __init__(self, parm):
                print('Constructed with "%s"' % parm)

        print("Exercising GeneralAutoLog with explicit function name that "
              "changes __init__ to the class name.")
        print("Should see:")
        print('<date    > <time  > --- (%s) BEGIN %s' % (logger_name, func_name))
        print('Constructed with "foo"')
        print('<date    > <time  > --- (%s) END   %s' % (logger_name, func_name))
        print('')
        MyClass('foo')
        print('')


class TestCase03AutoLoggerMember(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def test01_default_logger(self):
        logger_name = 'local_logging_tests'

        @GeneralAutoLog()
        def test_decorator(parm):
            print('Called with "%s"' % parm)

        func_name = test_decorator.__name__

        print("Exercising GeneralAutoLog with default logger.  Logger name defaults to module name")
        print("Should see:")
        print('<date    > <time  > --- (%s) BEGIN %s' % (logger_name, func_name))
        print('Called with "foo"')
        print('<date    > <time  > --- (%s) END   %s' % (logger_name, func_name))
        print('')
        test_decorator('foo')
        print('')


if __name__ == '__main__':
    unittest.main()
