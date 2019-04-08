#!/usr/bin/env python
"""Tests string_utilis.py
"""
import unittest
from string_utils import su


class TestCase(unittest.TestCase):
    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def test_stripSuffixes(self):
        self.assert_(su.stripSuffixes(["foo.bar", "all.out"]) == ["foo", "all"])

    def test_stripSuffix(self):
        self.assert_(su.stripSuffix("foo.bar") == "foo")

    def test_countIsOrAre(self):
        self.assert_(su.countIsOrAre(0) == "0 are")
        self.assert_(su.countIsOrAre(1) == "1 is")
        self.assert_(su.countIsOrAre(2) == "2 are")

    def test_countWasOrWere(self):
        self.assert_(su.countWasOrWere(0) == "0 were")
        self.assert_(su.countWasOrWere(1) == "1 was")
        self.assert_(su.countWasOrWere(2) == "2 were")

    def test_countSingularOrPlural(self):
        self.assert_(su.countSingularOrPlural(0, "is singular", "is plural") == "0 is plural")
        self.assert_(su.countSingularOrPlural(1, "is singular", "is plural") == "1 is singular")
        self.assert_(su.countSingularOrPlural(2, "is singular", "is plural") == "2 is plural")

    def test_noneToEmpty(self):
        self.assert_(su.noneToEmpty(None) == "")
        self.assert_(su.noneToEmpty("foo") == "foo")

    def test_concatWSpace(self):
        self.assert_(su.concatWSpace('one', 'two') == 'one two')
        self.assert_(su.concatWSpace('one', '') == 'one')
        self.assert_(su.concatWSpace('one', None) == 'one')
        self.assert_(su.concatWSpace('', 'two') == 'two')
        self.assert_(su.concatWSpace(None, 'two') == 'two')
        self.assert_(su.concatWSpace('', '') == '')
        self.assert_(su.concatWSpace('', None) == '')
        self.assert_(su.concatWSpace(None, '') == '')
        self.assert_(su.concatWSpace(None, None) == '')

    def test_concatWString(self):
        self.assert_(su.concatWString('one', "-", "two") == "one-two")
        self.assert_(su.concatWString('one', "...", "two") == "one...two")
        self.assert_(su.concatWString('one', "", "two") == "onetwo")
        self.assert_(su.concatWString('one', "...", '') == 'one')
        self.assert_(su.concatWString('one', "...", None) == 'one')
        self.assert_(su.concatWString('', "...", 'two') == 'two')
        self.assert_(su.concatWString(None, "...", 'two') == 'two')
        self.assert_(su.concatWString('', "...", '') == '')
        self.assert_(su.concatWString('', "...", None) == '')
        self.assert_(su.concatWString(None, "...", '') == '')
        self.assert_(su.concatWString(None, "...", None) == '')


if __name__ == '__main__':
    unittest.main()
