import Data.Maybe
import Data.List

import Distribution.Simple
import Distribution.Simple.Setup
import Distribution.PackageDescription

-- The purpose of this install hook is to rewrite the extra-lib-dirs and
-- ld-options to point to the libraries in the install directory.
-- This is disabled when we do an inplace install because this type of
-- install is used to do in-build-directory testing.
myInstallHook pkg_descr localbuildinfo hooks flags = defaultInstallHook newpkg_descr localbuildinfo hooks flags
 where
	defaultInstallHook = instHook simpleUserHooks
	inPlace = fromFlag (installInPlace flags)
	pkgLibrary = fromJust (library pkg_descr)
	pkgLibBuildInfo = libBuildInfo pkgLibrary
	newpkg_descr = if inPlace then pkg_descr else pkg_descr {
	     library = Just (pkgLibrary {
	          libBuildInfo = pkgLibBuildInfo {
	               extraLibDirs = "/g/g15/bronevet/Compilers/ROSE/compile/lib" : filter (not . isSuffixOf ".libs") (extraLibDirs pkgLibBuildInfo),
	               ldOptions = ["-Wl,-rpath,/g/g15/bronevet/Compilers/ROSE/compile/lib"]
	          }
	     })
	}

myHooks = simpleUserHooks {
     instHook = myInstallHook
}

main = defaultMainWithHooks myHooks
