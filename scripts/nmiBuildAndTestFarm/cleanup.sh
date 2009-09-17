
echo "Cleaning up old tarballs and generated environments"

# submit.sh creates temporary environment and tarball files
# clean up each one that is older than 72 hours
for temp_dir in `find $HOME/*nmi/{env,rose-tarballs} -mindepth 1 -maxdepth 1 -type d`; do
    touch -d '-72 hour' limit

    if [ limit -nt $temp_dir ]; then
        rm -fr $temp_dir
    fi

    rm -f limit
done

