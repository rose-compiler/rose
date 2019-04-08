#!/usr/bin/env python
""" Various convenience string manipulation functions.
"""


class StringUtils:
    def stripSuffixes(self, files):
        "Removes the suffix from a list of file names, so that foo.omf and foo.out match"
        return [self.stripSuffix(file) for file in files]

    def stripSuffix(self, name):
        "Removes the suffix from a file name, so that foo.omf and foo.out match"
        return name.split('.')[0]

    def countIsOrAre(self, count):
        "Returns 'is' if Count is 1, otherwise returns 'are'"
        return self.countSingularOrPlural(count, "is", "are")

    def countWasOrWere(self, count):
        "Returns 'was' if Count is 1, otherwise returns 'were'"
        return self.countSingularOrPlural(count, "was", "were")

    def countSingularOrPlural(self, count, singularString, pluralString):
        "Returns singularString if Count is 1, otherwise returns pluralString"
        if count == 1:
            return str(count) + " " + singularString
        else:
            return str(count) + " " + pluralString

    def concatWSpace(self, left, right):
        """ Concatenates two strings.  Puts a space between if neither is None or empty.
        """
        return self.concatWString(left, ' ', right)

    def concatWString(self, left, separator, right):
        """ Concatenates two strings.  Puts separator between if neither is None or empty.
        """
        if left and right:
            return separator.join((left, right))
        else:
            return self.noneToEmpty(left) + self.noneToEmpty(right)

    def noneToEmpty(self, stringIn):
        """ Given None, returns "", otherwise returns what it's given.
        """
        if stringIn is None:
            return ''
        else:
            return stringIn

    cws = concatWSpace
    cwst = concatWString


su = StringUtils()
