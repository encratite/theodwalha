import os
import nil.build, nil.environment, nil.setup

project = 'theodwalha'

nil.environment.unix_like()

builder = nil.build.builder(project)
for library in ['boost_system', 'boost_thread', 'boost_filesystem', 'ail', 'z', 'dl']:
	builder.library(library)

if builder.program():
	if builder.static_library(True):
		nil.setup.include(project)
		nil.setup.library(os.path.join(builder.output_directory, builder.library))
