#!/usr/bin/perl
use strict;
use Config;
use FindBin;
use lib "$FindBin::Bin";
use TupScripts;

sub test_first_file_component {
    my($comp, $sep, $rest);

    TupScripts::osname("windows");
    ($comp, $sep, $rest) = TupScripts::first_file_component("\\foo\\bar");
    die unless $comp eq "";
    die unless $sep eq "\\";
    die unless $rest eq "foo\\bar";
    TupScripts::osname("linux");

    ($comp, $sep, $rest) = TupScripts::first_file_component("/foo/bar");
    die unless $comp eq "";
    die unless $sep eq "/";
    die unless $rest eq "foo/bar";

    ($comp, $sep, $rest) = TupScripts::first_file_component("foo/bar");
    die unless $comp eq "foo";
    die unless $sep eq "/";
    die unless $rest eq "bar";

    ($comp, $sep, $rest) = TupScripts::first_file_component("bar");
    die unless $comp eq "bar";
    die unless $sep eq "";
    die unless $rest eq "";

    ($comp, $sep, $rest) = TupScripts::first_file_component("");
    die unless $comp eq "";
    die unless $sep eq "";
    die unless $rest eq "";
}

sub test_last_file_component {
    my($comp,$sep) = TupScripts::last_file_component("/foo/bar");
    die unless $comp eq "bar";
    die unless $sep eq "";
}

sub test_all_components {
    my(@parts) = TupScripts::all_components("/foo/bar");
    die unless @parts == 3;

    die unless $parts[0][0] eq "";
    die unless $parts[0][1] eq "/";

    die unless $parts[1][0] eq "foo";
    die unless $parts[1][1] eq "/";

    die unless $parts[2][0] eq "bar";
    die unless $parts[2][1] eq "";
}

sub test_canonical_name {
    my($s) = TupScripts::canonical_name("");
    die unless $s eq ".";

    $s = TupScripts::canonical_name("/foo/bar");
    die $s unless $s eq "/foo/bar";

    $s = TupScripts::canonical_name("/");
    die $s unless $s eq "/";

    $s = TupScripts::canonical_name(".");
    die $s unless $s eq ".";

    $s = TupScripts::canonical_name("/foo/./bar");
    die $s unless $s eq "/foo/bar";

    $s = TupScripts::canonical_name("/foo/../bar");
    die $s unless $s eq "/bar";

    $s = TupScripts::canonical_name("/foo//bar");
    die $s unless $s eq "/foo/bar";

    $s = TupScripts::canonical_name("foo/");
    die $s unless $s eq "foo";

    $s = TupScripts::canonical_name("..");
    die $s unless $s eq "..";
}

sub test_directory_name {
    my($s);

    $s = TupScripts::directory_name("/foo/bar");
    die $s unless $s eq "/foo";

    $s = TupScripts::directory_name("/foo/bar/");
    die $s unless $s eq "/foo/bar";

    $s = TupScripts::directory_name("/foo");
    die $s unless $s eq "";

    $s = TupScripts::directory_name("/");
    die $s unless $s eq "";

    $s = TupScripts::directory_name("./foo");
    die $s unless $s eq ".";

    $s = TupScripts::directory_name("foo");
    die $s unless $s eq ".";
}

sub test_base_name {
    my($s);

    $s = TupScripts::base_name("/foo/bar");
    die $s unless $s eq "bar";

    $s = TupScripts::base_name("/foo/bar/");
    die $s unless $s eq "bar";

    $s = TupScripts::base_name(".");
    die $s unless $s eq ".";
}

sub test_absolute_name {
    my($s);

    $s = TupScripts::absolute_name("/foo", "/root");
    die $s unless $s eq "/foo";

    $s = TupScripts::absolute_name("foo", "/root");
    die $s unless $s eq "/root/foo";
}

sub test_cwd_build_relative {
    my($s);

    $s = TupScripts::cwd_build_relative("../..", "/foo/bar/baz");
    die $s unless $s eq "bar/baz";
}

sub test_make_group_name {
    my($s);

    $s = TupScripts::make_group_name("foo");
    die $s unless $s eq "<foo>";

    $s = TupScripts::make_group_name("foo/bar");
    die $s unless $s eq "foo/<bar>";
}

test_first_file_component;
test_last_file_component;
test_all_components;
test_canonical_name;
test_directory_name;
test_base_name;
test_absolute_name;
test_cwd_build_relative;
test_make_group_name;
