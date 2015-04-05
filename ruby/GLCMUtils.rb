#!/usr/bin/ruby

class Array2D
	attr_reader :width, :height
	attr_accessor :data

	def initialize(d1, d2)
		@width = d1
		@height = d2
		if(block_given?) then
			@data = Array.new(d1) { |x| Array.new(d2) { |y| yield x, y } }
		else
			@data = Array.new(d1) { Array.new(d2) }
		end
	end

	def [](x, y)
		@data[x][y]
	end

	def []=(x, y, value)
		@data[x][y] = value
	end

	def each_index
		(0...@width).each do |x|
			(0...@height).each do |y|
				yield x, y
			end
		end
	end

#	def self.new_ppm(p1, p2, m1)
#		Array2D.new(p1.width, p1.height) { |x, y| p1[x, y] + p2[x, y] - m1[x, y] }
#	end

	def display
		(0...@height).each do |y|
			(0...@width).each do |x|
				$stderr.print "#{@data[x][y]}, "
			end
			$stderr.puts ""
		end
	end
end

class GLCM < Array2D
	attr_reader :size
	def initialize(size)
		if(block_given?) then
			super(size, size) { |x, y| yield x, y }
		else
			super(size, size) { |x, y| 0 }
		end
		@size = size
	end

	def self.make_another(other)
		GLCM.new(other.size) { |x, y| other[x, y] }
	end

	# plus plus minus
	# Compute p1 + p2 - m1
	def self.new_ppm(p1, p2, m1)
		GLCM.new(p1.size) { |x, y| p1[x, y] + p2[x, y] - m1[x, y] }
	end

	# plus minus minus plus
	# Compute p1 - m1 - m2 + p2
	def self.new_pmmp(p1, m1, m2, p2)
		GLCM.new(p1.size) { |x, y| p1[x, y] + p2[x, y] - m1[x, y] - m2[x, y] }
	end

	def add(x, y, value)
		@data[x][y] = @data[x][y] + value
	end

	def reset
		@data.each do |column|
			column.fill 0
		end
	end
end

# Object used to represent a region of an image, and to check if some coordinates are in or out
class Rectangle
	def initialize(_x1, _y1, _x2, _y2)
		@x1, @x2 = [_x1, _x2].sort
		@y1, @y2 = [_y1, _y2].sort
	end

	def include?(_x, _y)
		return (_x >= @x1) && (_x <= @x2) && (_y >= @y1) && (_y <= @y2)
	end

	def each
		(@x1..@x2).each do |x|
			(@y1..@y2).each do |y|
				yield x, y
			end
		end
	end
end

def max(i, j)
	return i > j ? i : j
end

def min(i, j)
	return i < j ? i : j
end

# Tests
if $0 == __FILE__
	puts "m1"
	m1 = GLCM.new(2) { |x, y| y * 2 + x }
	m1.display

	puts "m2"
	m2 = GLCM.new(2) { |x, y| (y * 2 + x) * 2 }
	m2.display

	puts "m3"
	m3 = GLCM.new(2) { |x, y| (y * 2 + x) * 4 }
	m3.display


	puts "m4"
	m4 = GLCM.new_ppm(m1, m3, m2)
	m4.display
end
