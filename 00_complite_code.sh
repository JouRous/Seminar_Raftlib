# Check PKG_CONFIG_PATH
echo ">> Kiểm tra xem PKG_CONFIG_PATH đã có đường dẫn đến raftlib chưa "
echo $PKG_CONFIG_PATH
echo ""

echo ">> Thêm đường dẫn vào PKG_CONFIG_PATH to /usr/local/pkgconfig"
export PKG_CONFIG_PATH=/usr/local/pkgconfig
echo ""

echo ">> Kiểm tra lại lần nữa PKG_CONFIG_PATH "
echo $PKG_CONFIG_PATH
echo ""

# Run
echo ">> Biên dịch code sử dụng thư viện raft"
echo ""

g++ `pkg-config --cflags raftlib` stream_server.cpp -o stream_server `pkg-config --libs raftlib`

echo ">> Xong, Chạy code bằng: ./stream"
echo ""
