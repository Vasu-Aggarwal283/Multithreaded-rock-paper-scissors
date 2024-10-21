project('rock_paper_scissors', 'cpp', version: '0.1.0')

# Find SFML components
sfml_system = dependency('sfml-system', required: true)
sfml_window = dependency('sfml-window', required: true)
sfml_graphics = dependency('sfml-graphics', required: true)

# Create executables for the client and server
client_executable = executable('client', 'client.cpp', dependencies: [sfml_system, sfml_window, sfml_graphics])
server_executable = executable('server', ['server.cpp', 'gameLogic.cpp'], dependencies: [sfml_system, sfml_window, sfml_graphics])
