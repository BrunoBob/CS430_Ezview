Simple program made by Bruno Tessier for CS430 class:

This program use opengl to load a ppm image (P6) and let the user transform this image (translation, rotation, scale and shear).

(require opengl es 2 and glfw3)
How to install : 	git clone https://github.com/BrunoBob/CS430_Ezview
			make

How to launch :		./ezview name_of_ppm_image

Commands :

	- Translation on X and Y axis : directional arrows
	
	- Rotation (normal keyboard and keypad are working):	- 1 & 3 for Z axis
								- 4 & 6 for Y axis
								- 2 & 8 for X axis

	-Scale : 	ALT + up or down

	-Shear :	CTRL + directional arrows
