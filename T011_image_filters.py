"""Team T011
#Consists of: Lorien Aquarius, And Others
#Submitted 06/02/2021
"""

from Cimpl import *
import numpy as np
from simple_Cimpl_filters import grayscale

def red_channel(original_image: Image)-> Image:
    """Returns a image with a red filtered applied to it 
    by: TeamMate
    """
    
    red_image = copy(original_image)
    for x, y, (r, g, b) in original_image:
        new_colour = create_color(r,0,0)
        set_color (red_image, x, y, new_colour)
    return red_image 

def green_channel(image : Image) -> Image:
    """
    returns a copy of the image that only contains the green value of the
    input image. Precondition: the image must be .png file type. 
    by: TeamMate 
    
    >>> original_image = load_image(choose_file()) # p2-original.png is chosen
    >>> green_image = green_channel(original_image)
    >>> show(green_image)
    >>> save_as(green_image, "p2-blue_filter.png")
    
    >>> original_image = Cimpl.load_image(Cimpl.choose_file()) # p2-original.png is chosen
    >>> blue_image = blue_channel(original_image)
    >>> show(blue_image)
    >>> save_as(green_image, "p2-blue_filter.png")

    """
    
    product_image = copy(image)
    
    for x, y, (r, g, B) in product_image:
        origin_color = get_color(image,x,y)
        red,green,blue = origin_color
        new_color = create_color(0,green,0)
        set_color(product_image,x,y,new_color)
    return product_image

def blue_channel(image: Image) -> Image:
    """ Returns a copy of the original image with the red and green components of each pixel set to zero, while the blue component stays the same.
    by: TeamMate

    >>> original_image = load_image(choose_file()) # p2-original.png is chosen
    >>> blue_image = blue_channel(original_image)
    >>> show(blue_image)
    >>> save_as(blue_image, "p2-blue_filter.png")
    
    >>> original_image = load_image(choose_file()) # green_image.png is chosen
    >>> blue_image = blue_channel(original_image)
    >>> show(blue_image)
    >>> save_as(blue_image, "blue_filter.png")
    
    """
   
    image_copy = copy(image)
    height = get_height(image)
    width = get_width(image)
    for y in range(0, height):
        for x in range(0, width):
            pixel_colour = get_color(image_copy, x, y)
            r, g, b = pixel_colour
            r = 0
            g = 0 
            if b > 255:
                b == 255
            elif b < 0:
                b == 0
            blue_filter = create_color(r, g, b)        
            set_color(image_copy, x, y, blue_filter)
    return image_copy

#Constants
RED = create_color(255, 0, 0)
GREEN = create_color(0, 255, 0)
BLUE = create_color(0, 0, 255)
    
#Function to combine 3 different monochromatic images into one full colour image
def combine_rgb(red_image:Image, green_image:Image, blue_image:Image) -> Image:
    """T011 written by: Lorien Aquarius #101189924
    Returns an image of combined Red, Green, and Blue monochromatic images
    >>>red = load_image(choose_file())
    >>>green = load_image(choose_file())
    >>>blue = load_image(choose_file())
    >>>combined = combine_rgb(red, green, blue)
    >>>show(combined)
    """
    
    #Initialize the new image object. This could be done with any of the three images
    new_image = copy(red_image)
    
    #Iterate through each pixel in the 3 images, using the new image
    for x, y, (r, g, b) in new_image:
        
        #Get the colour of each pixel
        red_color = red_image.get_color(x, y)[0] #Can retrieve values like this due to colour objects extending tuples
        green_color = green_image.get_color(x, y)[1]
        blue_color = blue_image.get_color(x, y)[2]
        
        #Set the colour of the current pixel to the combined RGB values collected from the monochromatic images
        set_color(new_image, x, y, create_color(red_color, green_color, blue_color))
    
    #Return the final created image
    return new_image

def draw_curve(image: Image, colour: str, pointlist: list = None) -> tuple:
    """
    Author: TeamMate
    
    Returns a tuple containing a copy of the image with a specified coloured curve drawn to connect the points entered, as well as a list with the points of the curve on the border of the image.  
    When the pointlist parameter is None, the user will have to enter the desired points while the program executes.
    The pointlist parameter can also take in a list of points to skip the input steps in the code. 
    
    >>> original_image = load_image("great_big_c.jpg")
    >>> show(original_image)
    >>> curve_image = draw_curve(original_image, "magenta", None) #user inputs desired points when executing the code
    >>> show(curve_image[0]) #image with curve is shown
    >>> print("The curve intersects the border of the image at: " + str(curve_image[1]))
    The curve intersects the border of the image at: [(0, 2), (119, 479)]
    >>> save_as(curve_image[0], "great_big_c_curve.jpg")
    
    >>> original_image = load_image("miss_sullivan.jpg")
    >>> show(original_image)
    >>> curve_image = draw_curve(original_image, "cyan", [(0,2), (52, 210), (170, 500)]) #points are in function call
    >>> show(curve_image[0]) #image with curve is shown
    >>> print("The curve intersects the border of the image at: " + str(curve_image[1])) 
    The curve intersects the border of the image at: [(0, 2), (156, 479)]
    >>> save_as(curve_image[0], "miss_sullivan_curve.jpg")
    
    """
    
    curve_colour = colour_rgb(colour)
    curve_image = copy(image)
    pixel_y = get_height(curve_image)
    pixel_x = get_width(curve_image)  
    image_size = [pixel_x, pixel_y]
    
    if pointlist == None:
        print("The image dimensions are (x,y): " + str((pixel_x, pixel_y)))
        number = int(input("Please enter the number of points desired (must be at least 2): "))  
        point_count = 1
        points_list = []
        while point_count <= number:
            x_coordinate = int(input("Enter point " + str(point_count) + " x-coordinate: "))
            y_coordinate = int(input("Enter point " + str(point_count) + " y-coordinate: "))
            points_list += [(x_coordinate, y_coordinate)]
            point_count += 1
        ascending_list = sorted(points_list)    
    else:
        ascending_list = sorted(pointlist)
    print("The sorted entered points are: " + str(ascending_list))
    
    polycoeff = _interpolation(ascending_list)
    edge_points_list = image_border_finding(image_size, polycoeff)    
    
    for x in range(pixel_x):
        curve_width = -2
        for value in range(5):
            if 0 <= x <= (pixel_x - 1) and 0 <= (np.polyval(polycoeff, x) + curve_width) <= (pixel_y - 1):
                set_color(curve_image, x, (np.polyval(polycoeff, x) + curve_width), curve_colour)
            curve_width += 1 
    
    return (curve_image, edge_points_list)
    
       
           
def colour_rgb(colour: str) -> Color:
    """
    Author: TeamMate
    
    Creates the curve colour corresponding to the red, green, and blue values of the colour name entered by the user.
    
    >>> colour_rgb("green")
    (0,255,0)
    
    >>> colour_rgb("lemon")
    (255,255,0)
    
    """
    
    colour_dictionary = {"black":(0,0,0), "white":(255,255,255), "blood":(255,0,0), 
               "green":(0,255,0), "blue":(0,0,255), "lemon":(255,255,0), 
               "cyan":(0,255,255), "magenta":(255,0,255), "gray":(128,128,128)}
    if colour in colour_dictionary:
        curve_colour = colour_dictionary.get(colour)
        curve_colour = create_color(curve_colour[0], curve_colour[1], curve_colour[2])
    return curve_colour



def _interpolation(ascending_list: list) -> list:
    """
    Author: TeamMate
    
    Returns a list of coefficients, sorted from highest degree to lowest degree term, corresponding to the polynomial created from the list of points provided by the user.  If three or less points are entered, the coefficients will correspond to an interpolating polynomial. If more than three points are entered, the coefficients will correspond to a quadratic regression polynomial.
    
    >>> _interpolation([(0,0), (3, 56), (40, 207)])
    [-0.36463963963964113, 19.760585585585634, -1.640928159047308e-13]
    
    >>> _interpolation([(0,0), (640, 480)]
    [0.7499999999999998, 2.9997383549831188e-15]
    
    """
    
    x_vals = []
    y_vals = []
    n = len(ascending_list)
    for (x,y) in ascending_list:
        x_vals += [x]
        y_vals += [y]
    x_points = np.array(x_vals)
    y_points = np.array(y_vals)
    
    if n <= 3:
        polycoeff = list(np.polyfit(x_points, y_points, n-1))
    else:
        polycoeff = list(np.polyfit(x_points, y_points, 2))
       
    return polycoeff



def image_border_finding(image_size: list, polycoeff: list) -> list:
    """
    Author: TeamMate
    
    Returns a list containing the pixels where the curve intersects the border of the image, sorted from smallest to greatest x-value when given the dimensions of the image (x,y) and a list of the polynomial coefficients.
    
    >>> image_border_finding([640,480], [-0.36463963963964113, 19.760585585585634, -1.640928159047308e-13])
    [(0,0])
    
    >>> image_border_finding([640,480], [7.24e-04,-1.19e+00,4.51e+02])
    [(0, 451.0), (590, 0)]
    
    """
    
    pixel_x, pixel_y = image_size
    edge_points = []
    
    left_edge = np.polyval(polycoeff, 0) #Checks the y-value of the function at x = 0.
    right_edge = np.polyval(polycoeff, pixel_x - 1) #Checks the y-value of the function at x = 640.
    bottom_edge = _exhaustive_search(pixel_x - 1, polycoeff, 0) #Determines x-value when f(x) = 0.
    top_edge = _exhaustive_search(pixel_x - 1, polycoeff, pixel_y - 1) #Determines the x-value when f(x) = 480.
    
    if 0.001 <= left_edge <= (pixel_y - 1): #Checks if the y-value is in the image dimensions.
        if left_edge - int(left_edge) > 0.99: #Due to the floats returned by the polyval function, the value will be rounded up to the nearest integer if the difference is above 0.99.
            left_edge = int(left_edge) + 1 #Rounds the float up to the nearest integer.
        else:
            left_edge = int(left_edge) #Rounds the float down to the neareset integer.
        edge_points += [(0, left_edge)]
    if 0.001 <= right_edge <= (pixel_y -1): #Checks if the y-value is in the image dimensions.
        if right_edge - int(right_edge) > 0.99:
            right_edge = int(right_edge) + 1
        else:
            right_edge = int(right_edge)        
        edge_points += [(pixel_x - 1, right_edge)]
    if bottom_edge != None: #Will only add the coordinate to the edge_point_list if the x-value is within the image dimensions and therefore not a None type.
        edge_points += [(bottom_edge, 0)]
    if top_edge != None:
        edge_points += [(top_edge, pixel_y - 1)]
    
    return sorted(edge_points)
    
    

def _exhaustive_search(max_x: int, polycoeff: list, val: int) -> int:
    """
     Author: TeamMate
     
     Solves f(x)-val=0 for x between 0 and max_x where polycoeff contains the coefficients of f, using EPSILON of 1 (as we only need ints for pixels).
     Returns None if there is no solution between the bounds.

     >>> _exhaustive_search(639,[6.33e-03,-3.80e+00,5.57e+02],0)
     253
     >>> _exhaustive_search(639,[7.24e-04,-1.19e+00,4.51e+02],0)
     590
     >>> _exhaustive_search(639,[7.24e-04,-1.19e+00,4.51e+02],479)
     None
     
    """
    
    EPSILON = 1
    ERROR_MARGIN = 0.1 #Margin of error that accounts for the polyfit and polyval function inaccuracies.
    step = EPSILON
    x_root_guess = 0
    function = np.polyval(polycoeff, x_root_guess) 
    
    while abs(function - val) >= (EPSILON + ERROR_MARGIN) and x_root_guess <= max_x: 
        x_root_guess += step
        function = np.polyval(polycoeff, x_root_guess) 
        
    if x_root_guess > max_x:
        return None
    else:
        return x_root_guess

def flip_vertical (original_image:Image) -> Image:
    """
    Made by TeamMate
    Returns a picture that is vertically flipped 
    """

    horizontal_image = copy (original_image)
                        
    width = get_width (original_image)
    height = get_height (original_image)
    
    for x in range (width):
        for y in range (height):
            color = get_color (original_image, x, y) 
            flip = (height - 1)-y     #Takes each pixel and flips over a horizontal line
            set_color (vertical_image,x,flip,color)  #Sets the color for the new image
            
    return vertical_image

def detect_edges(image : Image, threshold : int) -> Image:
    """
    Author: TeamMate 
    returns a copy of the image where depending on the threshold, will change the pixels to black or white. 
    

    """
    product_image = copy(image)
    blk = create_color(0,0,0)
    wte = create_color(255,255,255)
    
    height = get_height(product_image)
    
    for x, y, (r, g, B) in product_image:
        top = get_color(image,x,y)
        rtop,gtop,btop = top
        
        if y < height -1:
            btm = get_color(image,x,y+1)
            rbtm,gbtm,bbtm = btm
        
            topbright = (rtop + gtop + btop)/3 
            btmbright = (rbtm + gbtm + bbtm)/3
        
            contrast = abs(topbright - btmbright)
        
            if contrast > threshold:
                set_color(product_image, x, y, blk)
            else:
                set_color(product_image,x,y,wte)
        
        elif y == height -1:
            set_color(product_image,x,y,wte)
        
        
    return product_image

def flip_horizontal(original: Image) -> Image:
    """T011 by: Lorien Aquarius 101189924
    Returns the same image given flipped along a vertical axis
    
    >>>your_image = flip_vertical(my_image)
    """
    flipped = copy(original) #Copy the initial image
    coordinate_height = flipped.get_height() #get the height of the image
    coordinate_width = flipped.get_width() #get the width of the image
     
    for x in range(0, (coordinate_width // 2)): #iterate through the x coordinates until half the image is reached
        for y in range(coordinate_height): #iterate through the y coordinates of the image
            left_colour = get_color(flipped, x, y) #get the colour of the left side
            right_colour = get_color(flipped, coordinate_width - x - 1, y) #get the colour of the right side
            
            set_color(flipped, x, y, right_colour) #set the left colour to that of the right
            set_color(flipped, coordinate_width - x - 1, y, left_colour) #set the right colour to that of the left
    
    return flipped #return the flipped image

def three_tone(image : Image, color1 : str , color2 : str , color3 : str) -> Image:
    
    """
    Author: TeamMate  
    
    returns a copy of the image that changes the color for
    
    four parameters , an image and three strings( names of colors). returns copy of imagewith only those three colors. 0-84-> color 1; 85-170 = color2 ; 170-255 = colour 3
    """
    colors = ["black","white", "blood", "green", "blue","lemon","cyan","magenta", "gray"]
    create = [create_color(0,0,0) , create_color(255,255,255), create_color(255,0,0), create_color(0,255,0), create_color(0,0,255), create_color(255,255,0), create_color(0,255,255), create_color(255,0,255),create_color(128,128,128)]
    i = 0
    while i < len(colors): 
        if colors[i] == color1:
            ccolor1 = create[i]
            
        if colors[i] == color2:
            ccolor2 = create[i]

        if colors[i] == color3:
            ccolor3 = create[i]
        i += 1
        
    
    
    product_image = copy(image)
    
    for x, y, (r, g, B) in product_image:
        origin_color = get_color(image,x,y)
        red,green,blue = origin_color
        
        if 0 <= (red + green + blue) / 3 < 85: 
        
            set_color(product_image,x,y,ccolor1)
            
        elif 85 <= (red + green + blue) / 3 < 171: 
        
            set_color(product_image,x,y,ccolor2)
            
        elif 171 <= (red + green + blue) / 3 <= 255: 
        
            set_color(product_image,x,y,ccolor3)
            
    return product_image    
    

def extreme_contrast(image: Image) -> Image:
    """ Returns a copied image which has been modified to maximize the contrast of each pixel.  
    If the red, green, or blue component ranges from 0 to 127, then it is decreased to 0.
    If the red, green, or blue component ranges from 128 to 255, then it is increased to 255.
    
    >>> original_image = load_image("riveter.jpg")
    >>> show(original_image)
    >>> contrast_image = extreme_contrast(original_image)
    >>> show(contrast_image)
    >>> save_as(contrast_image, "riveter_contrast_filter.jpg")
    
    >>> original_image = load_image("great_big_c.jpg")
    >>> show(original_image)
    >>> contrast_image = extreme_contrast(original_image)
    >>> show(contrast_image)
    >>> save_as(contrast_image, "great_big_c_contrast_filter.jpg")
    
    """
    
    image_copy = copy(image)
    height = get_height(image)
    width = get_width(image)
    for y in range(0, height):
        for x in range(0, width):    
            pixel_colour = list(get_color(image_copy, x, y))
            index = 0
            for rgb_value in pixel_colour:
                if rgb_value <= 127:
                    rgb_value = 0
                elif 128 <= rgb_value:
                    rgb_value = 255
                pixel_colour[index] = rgb_value
                index += 1
            contrast_colours = create_color(pixel_colour[0], pixel_colour[1], pixel_colour[2])        
            set_color(image_copy, x, y, contrast_colours)                 
    return image_copy

def sepia_channel (original_image:Image) -> Image:
    """
    Made by TeamMate
    Retruns a image with a sepia filter applied to it 
    
    >>>original_image = load_image(choose_file())
    >>>sepia_image = sepia_channel(original_image)
    >>>show(sepia_image)
    
    """
    
    sepia_iamge = copy (original_image)
    grayscale_image = grayscale (original_image)
    
    for x, y, (r,g,b) in grayscale_image:
        
        if r < 63:
            b *= 0.9
            r *= 1.1
            
        elif r >= 63 and r <= 191:
            b *= 0.85
            r *= 1.15
            
        else:   
            b *= 0.93
            r *= 1.08
            if r > 255:    #Makes sure that the r value doesnt exceed 255 
                r = 255 
            
        new_color = create_color (r,g,b)
        set_color (grayscale_image, x, y, new_color)
        
    return grayscale_image 

def posterize(original: Image) -> Image:
    """T011 by: Lorien Aquarius #101189924
    Returns the posterized version of a given image
    >>>posterize(my_image)
    """
    #Create a new image based off the original
    posterized = copy(original)
    
    #Cycle through each pixel in the new image
    for x, y, (r, g, b) in posterized: #Cycle through all the pixels in a given image
        
    
        new_red = _adjust_component(r) #generate new red component of pixel
        new_green = _adjust_component(g) #generate new green component of pixel
        new_blue = _adjust_component(b) #generate new blue component of pixel
        
        new_color = create_color(new_red, new_green, new_blue) #Create new color object using the 3 determined rgb values
        
        set_color(posterized, x, y, new_color) #Set the current pixel to the processed colour
        
    return posterized #Return the processed image
        
def _adjust_component(colour_value: int) -> int:
    """Returns the midpoint of the posterize quadrant a colour value is in
    >>>_adjust_component(5)
    >>>31
    >>>_adjust_component(100)
    >>>95
    >>>_adjust_component(255)
    >>>2223
    """
    if colour_value <= 63: #First quadrant
        return 31 #midpoint
    elif colour_value <= 127: #Second quadrant
        return 95 #midpoint
    elif colour_value <= 191: #Third quadrant
        return 159 #midpoint
    else: #If not in the other quadrants, it's in the last quadrant
        return 223 #final midpoint

