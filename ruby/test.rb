#!/usr/bin/ruby

require 'RMagick'
load 'GLCMUtils.rb'

# Number of grey levels we want the image to be reduced to
quantization_level = 4

img_tmp = Magick::Image::read('img.bmp').first
img = Array2D.new(img_tmp.columns, img_tmp.rows)

# Reduce the number of gray levels
img_tmp.each_pixel do |pix, x, y|
	img[x, y] = (pix.red * quantization_level/65536.0).to_i # The image structure uses shorts and not chars, which explain the 65536
end

#img = Array2D.new(4, 4)
#img.data = [
#	[0, 0, 1, 1],
#	[0, 0, 1, 1],
#	[0, 2, 2, 2],
#	[2, 2, 3, 3]
#].transpose

Window = [2, 2] # Radius of a region on which the cooccurrence matrix will be computed (around each pixel)
Offset = [1, -1] # Offset used to define the cooccurrence
Target = [10, 10] # Coordinates of the pixel we want to compute the cooccurrence matrix

glcm = GLCM.new(quantization_level)

############################
# Original method
############################

img.each_index do |x, y|
	x1 = x - Window[0]
	y1 = y - Window[1]
	x2 = x + Window[0]
	y2 = y + Window[1]

	glcm.reset

	region = Rectangle.new(x1 >= 0 ? x1 : 0, y1 >= 0 ? y1 : 0, x2 < img.width ? x2 : img.width-1, y2 < img.height ? y2 : img.height-1)
	region.each do |k, l|
		ko = k + Offset[0]
		lo = l + Offset[1]

		next unless region.include?(ko, lo) 

		glcm.add(img[k, l], img[ko, lo], 1)
		glcm.add(img[ko, lo], img[k, l], 1)
	end

	if(x == Target[0] && y == Target[1]) then
		puts "Standard method"
		glcm.display
	end
end

############################
# Integral method
############################

glcm_integral = Array2D.new(img.width, img.height)
GLCM.new(quantization_level)

# Relatives coordinates to the two rightmost/bottom-most pixels
# linked by the offset (relatively to the rightmost/bottom-most
# pixel of a region)
# The first one must be negated
Offset1 = [max(0, Offset[0]), max(0, Offset[1])]
Offset2 = [min(0, Offset[0]), min(0, Offset[1])]

# The pixel at (0, 0)
glcm_integral[0, 0] = GLCM.new(quantization_level)

# first line
(1...img.width).each do |k|
	glcm = glcm_integral[k-1, 0].dup # duplicate

	image_bb = Rectangle.new(0, 0, k, 0) # BoundingBox

	k1 = k - Offset1[0]
	l1 = -Offset1[1]
	k2 = k + Offset2[0]
	l2 =  Offset2[1]
	if(image_bb.include?(k1, l1) && image_bb.include?(k2, l2)) then # Checking if the pixels are in the region
		glcm.add(img[k1, l1], img[k2, l2], 1)
		glcm.add(img[k2, l2], img[k1, l1], 1)
	end

	glcm_integral[k, 0] = glcm
end

# first column
(1...img.height).each do |l|
	glcm = glcm_integral[0, l-1].dup

	image_bb = Rectangle.new(0, 0, 0, l)

	k1 = -Offset1[0]
	l1 = l - Offset1[1]
	k2 =  Offset2[0]
	l2 = l + Offset2[1]
	if(image_bb.include?(k1, l1) && image_bb.include?(k2, l2)) then
		glcm.add(img[k1, l1], img[k2, l2], 1)
		glcm.add(img[k2, l2], img[k1, l1], 1)
	end

	glcm_integral[0, l] = glcm
end

# The remaining pixels
(1...img.width).each do |k|
	(1...img.height).each do |l|
		# Compute the cooccurrence matrix of the (0, 0) -> (k, l) region based on the integral image principle
		glcm = GLCM.new_ppm(glcm_integral[k-1, l], glcm_integral[k, l-1], glcm_integral[k-1, l-1])
		image_bb = Rectangle.new(0, 0, k, l)

		k1 = k - Offset1[0]
		l1 = l - Offset1[1]
		k2 = k + Offset2[0]
		l2 = l + Offset2[1]
		if(image_bb.include?(k1, l1) && image_bb.include?(k2, l2)) then
			glcm.add(img[k1, l1], img[k2, l2], 1)
			glcm.add(img[k2, l2], img[k1, l1], 1)
		end

		glcm_integral[k, l] = glcm
		if(k == Target[0] + Window[0] && l == Target[1] + Window[1]) then
			# We have calculated everything we need to compute the cooccurrence matrix for the target pixel
			puts "Integral method"

			# Coordinates of the region that will be used to compute the cooccurrence matrix
			x1 = Target[0] - Window[0]
			y1 = Target[1] - Window[1]
			x2 = Target[0] + Window[0]
			y2 = Target[1] + Window[1]

			# Computing the cooccurrence matrix for the region based on the integral image principle.
			# The is some other offsets to handle due to the direction and size of the main offset.
			# This does not handle the case where the target pixel is on the first row or column.
			GLCM.new_pmmp(glcm_integral[x2, y2], glcm_integral[x2, y1 - 1 + Offset[1].abs], glcm_integral[x1 - 1 + Offset[0].abs, y2], glcm_integral[x1 - 1 + Offset[0].abs, y1 - 1 + Offset[1].abs]).display
		end
	end
end

