import nil.build

project = 'theodwalha'

builder = nil.build.builder(project)
for library in ['boost_system', 'boost_thread', 'boost_filesystem', 'ail', 'z', 'dl']:
	builder.library(library)
builder.program()
