# Justfile

# Define a variable for the build directory
builddir := "build"

# Command to build the server and client
build:
    meson setup {{builddir}} && meson compile -C {{builddir}}

# Command to run the server
run-server:
    cd {{builddir}} && ./server

# Command to run the client
run-client:
    # Ensure the fonts directory is in the right place
    mkdir -p {{builddir}}/fonts
    cp fonts/DejaVuSans.ttf {{builddir}}/fonts/
    
    cd {{builddir}} && ./client

# Clean the build
clean:
    rm -rf {{builddir}}
