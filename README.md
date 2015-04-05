# IGLCM

This code demonstrates how to compute [gray level cooccurrence matrices](http://en.wikipedia.org/wiki/Co-occurrence_matrix) for regions centered around each pixel of an image using the [integral image](http://en.wikipedia.org/wiki/Summed_area_table) principle.

This principle allow to reduce the time required to compute all matrices, but may use a lot more memory, since we need to keep in memory a cooccurrence matrix for each pixel of the image.

## License

This tool is released under the terms of the MIT License. See the LICENSE.txt file for more details.
