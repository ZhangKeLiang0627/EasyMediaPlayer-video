export STAGING_DIR=/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target

make -j4

mkdir -p build

mv *.o build/
find ./src -type f -name "*.o" -exec mv {} ./build/ \;  