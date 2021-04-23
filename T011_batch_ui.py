#Programmer: Lorien Aquarius (Main Script) TeamMate (Execute Function)
#Group T011 task P5 Milestone 3
#Submitted: 22/02/2021
from Cimpl import choose_file, load_image, save_as
from T011_image_filters import *
def execute (operator: str, original_image: Image) -> Image:
    """Returns the result of a entered operator which corresponds to a filter to 
    change an image 
    Returns the same image with no changes if operator isnt defined 
    Author : TeamMate 
    """
    
    if operator == "3":
        return three_tone (original_image, "blood", "lemon", "gray")
    elif operator == "X":
        return extreme_contrast (original_image)
    elif operator == "T":
        return sepia_channel (original_image)
    elif operator == "P":
        return posterize (original_image)
    elif operator == "E":
        return detect_edges (original_image,15)
    elif operator == "V":
        return flip_vertical (original_image)
    elif operator == "H:":
        return flip_horizontal (original_image)
    else:
        return original_image
#Main Script by: Lorien Aquarius

path = choose_file() #Choose batch file

file = open(path, "r") #Open batch file to be read

for line in file: #Cycle through lines in the batch file
    contents = line.split(' ') #split the line into a list based on spaces
    initial_image = contents.pop(0) #Remove the first entry, which should be the name of the picture to be processed
    save_name = contents.pop(0) #Removes first entry, which should now be name of the file to be saved
    
    to_process = load_image(initial_image) #load in the image to be processed
    
    
    for process in contents: #Cycle through processes
        to_process = execute(process.upper(), to_process) #change the image
    
    save_as(to_process, save_name) #Save the image
    
    
file.close() #Close the file