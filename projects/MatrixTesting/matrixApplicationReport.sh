#!/bin/bash
# Report application test result by email
arg0="${0##*/}"

: "${MATRIX_EMAIL:=matzke1@llnl.gov}"
: "${MATRIX_SUBJECT:=application test result}"

die() {
    echo "$arg0:" "$@" >&2
    exit 1
}

# Switches
dry_run=
while [ "$#" -gt 0 ]; do
    case "$1" in
	--) shift; break ;;

	--email=*) MATRIX_EMAIL="${1#--email=}"; shift ;;
	--email) MATRIX_EMAIL="$2"; shift 2 ;;

	--dry-run) dry_run=":"; shift ;;

	-*) die "unknown switch: $1" ;;

	*) break ;;
    esac
done

email_body_file=
cleanup() {
    rm -f "$email_body_file"
}

trap cleanup EXIT

# This is the list of valid keys and whether they're required or optional
declare -A valid_keys
valid_keys=(
    [reporting_user]=optional
    [reporting_time]=optional
    [tester]=optional
    [os]=optional
    [rose]=required
    [rose_date]=required
    [application]=required
    [application_version]=required
    [application_date]=optional
    [nfiles]=required
    [npass]=required
    [duration]=optional
    [noutput]=optional
    [nwarnings]=optional
)

# These are the keys we've seen so far
declare -A seen_keys

# Return (echo) the OS name as a short string
os_short_name() {
    local os_name
    # Debian/Ubuntu-like systems
    [ "$os_name" = "" -a -r /etc/os-release ] && \
        os_name=$(source /etc/os-release; echo $NAME $VERSION_ID)

    # Other debian-like systems
    [ "$os_name" = "" -a -r /etc/debian_version ] && \
        os_name="Debian $(cat /etc/debian_version)"

    # Redhat-like systems
    [ "$os_name" = "" -a -r /etc/redhat-release ] && \
        os_name=$(cat /etc/redhat-release)

    # All others, fall back to the Linux kernel version
    [ "$os_name" = "" ] && \
        os_name="Unknown $(uname -s)"

    # Create a short OS name. E.g., instead of "Red Hat Enterprise Linux Workstation release 6.7 (Santiago)" we'll use
    # just "RHEL 6.7".
    echo "$os_name" |
	sed \
            -e 's/Red Hat Enterprise Linux Workstation release \([0-9]\+\.[0-9]\+\).*/RHEL \1/' \
            -e 's/Red Hat Enterprise Linux Server \([0-9]\+\.[0-9]\+\).*/RHEL \1/' \
	    -e 's/Red Hat Enterprise Linux Server release \([0-9]\+\.[0-9]\+\).*/RHEL \1/' \
            -e 's/Debian GNU.Linux/Debian/'
}

# Generate the body of an email, which consists of NAME=VALUE pairs, one per line.  We'll do some basic validation here
# so the user sees error messages, but the real validation occurs on the email server.
email_body() {
    while [ "$#" -gt 0 ]; do
	case "$1" in
	    *=*)
		# Keys are valid SQL non-quoted identifiers except underscores cannot appear at the beginning or end or
		# be doubled up. Underscores may, however, be replaced with hyphens.
		local key=$(echo "$1" |sed -n 's/^\([a-z]\+\([-_][a-z0-9]\+\)*\)=.*$/\1/p')
		[ "$key" = "" ] && die "malformed key in '$1'"
		[ -n "${valid_keys[$key]}" ] || die "invalid key '$key' in '$1'"

		# Values cannot have any characters that need shell or SQL string escapes. The email filter will be very
		# picky about this, so we might as well tell the user up front so they can see an error message.
		local value="${1#*=}"
		local value_is_okay=$(echo "$value" |sed -n 's/^[-_+@=:,./a-zA-Z0-9]*$/yes/p')
		[ -n "$value_is_okay" ] || die "invalid characters in value: $value"

		# Keys cannot be repeated
		[ -n "${seen_keys[$key]}" ] && die "key '$key' is specified more than once"
		seen_keys[$key]=yes

		echo "$1"
		shift
		;;

	    *) die "malformed argument: $1" ;;
	esac
    done
}

# Checks whether any required keys are absent from the command-line
check_required_keys_found() {
    local key
    for key in "${!valid_keys[@]}"; do
	if [ "${seen_keys[$key]}" = "" -a "${valid_keys[$key]}" = required ]; then
	    die "key '$key' is required but not specified"
	fi
    done
}

# Provide default values for optional keys that weren't specified
provide_default_values() {
    local key
    for key in "${!valid_keys[@]}"; do
	if [ "${valid_keys[$key]}" = "optional" -a "${seen_keys[$key]}" = "" ]; then
	    case "$key" in
		reporting_user|tester)
		    echo "$key=$(whoami)@$(hostname --fqdn)"
		    ;;
		reporting_time)
		    echo "$key=$(date +%s)"
		    ;;
		os)
		    echo "$key=$(os_short_name)"
		    ;;
	    esac
	fi
    done
}
		
email_body_file=$(tempfile)
email_body "$@" >$email_body_file
check_required_keys_found
provide_default_values >>$email_body_file

if [ -n "$dry_run" ]; then
    echo "$arg0: dry-run: following email would have been sent to $MATRIX_EMAIL"
    echo "    |Subject: $MATRIX_SUBJECT"
    sed 's/^/    |/' <$email_body_file
fi

(
    [ -n "$dry_run" ] && set -x
    $dry_run mail -s "application test result" "$MATRIX_EMAIL" <$email_body_file
)
