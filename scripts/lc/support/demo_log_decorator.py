class SomeClass(object):

    def write_begin_end(message):
        """Decorator factory, returning a begin-end decorator with the desired message.
        Note: Not a class method, just hidden in the class namespace
        """
        def decorator(func_to_be_wrapped):
            """Decorator, returning a wrapped function
            Note: Not a class method, just hidden in the class namespace
            """
            def wrapper(self, *args, **kwargs) :
                """The wrapped function (self is included in args*)
                """
                print ("Start {}".format(message))
                func_to_be_wrapped(self, *args, **kwargs)
                print ("End {}".format(message))
            return wrapper
        return decorator

    @write_begin_end("bar")
    def bar(self, parm ) :
        print (parm)

    @write_begin_end("bat")
    def bat(self, parm ) :
        print (parm)

test = SomeClass()

test.bar("parm1")
test.bat("parm2")
