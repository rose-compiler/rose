# test ellipses in slices

class foo:
    def __getitem__(self,id):
        print id

a = foo()
print a[...]
