#ifndef ROSE_StatSerializer_H
#define ROSE_StatSerializer_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB

#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>

#include <sys/types.h>

// Serialization for struct stat
namespace boost {
namespace serialization {

template<class S>
void serialize(S & s, struct stat &x, const unsigned /*version*/) {
    s & boost::serialization::make_nvp("st_dev",     x.st_dev);
    s & boost::serialization::make_nvp("st_ino",     x.st_ino);
    s & boost::serialization::make_nvp("st_mode",    x.st_mode);
    s & boost::serialization::make_nvp("st_nlink",   x.st_nlink);
    s & boost::serialization::make_nvp("st_uid",     x.st_uid);
    s & boost::serialization::make_nvp("st_gid",     x.st_gid);
    s & boost::serialization::make_nvp("st_rdev",    x.st_rdev);
    s & boost::serialization::make_nvp("st_size",    x.st_size);
    s & boost::serialization::make_nvp("st_blksize", x.st_blksize);
    s & boost::serialization::make_nvp("st_blocks",  x.st_blocks);
}

} // namespace
} // namespace

#endif
#endif
#endif
