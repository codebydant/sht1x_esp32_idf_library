set(srcs "main.c"
	"src/sht1x.c"
	
)

idf_component_register(SRCS ${srcs} INCLUDE_DIRS ".")




