# This is a script to erase all the test directories that accumulate during testing of ROSE.

TEST_DIR = ~/ROSE/svn-test-rose/

# As I recall the order is is important.
chmod -R +x $(TEST_DIR)/[1-9]*
chmod -R +r $(TEST_DIR)/[1-9]*
rm -rf $(TEST_DIR)/[1-9]*

# OR:
# chmod -R +x [1-9]*; chmod -R +r [1-9]*; rm -rf [1-9]*

