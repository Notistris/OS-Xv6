# user/xargstest.sh
# Script test xargs cho xv6

# Test đơn giản
echo hello | xargs echo
echo hello | xargs echo hello
echo hello | xargs echo hello hello

# Test nhiều dòng
(echo hello; echo hello) | xargs echo
(echo hello; echo hello) | xargs echo hello
(echo hello; echo hello) | xargs echo hello hello
