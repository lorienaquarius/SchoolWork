# Author: TeamMate
# Author: TeamMate
# T011 Milestone 3 P5 - Interactive User Interface
# Date of submission: 22/02/21

from T011_image_filters import *


def user_command() -> str:
    """ Author: TeamMate.
    Displays the menu of commands and returns the command selected by the user. """
    
    menu_options = "L)oad image  S)ave-as \n3)-tone  X)treme contrast  T)int sepia  P)osterize \nE)dge detect  D)raw curve  V)ertical flip  H)orizontal flip \nQ)uit \n \n: "
       
    user_input = str(input(menu_options)).upper()
    return user_input 


def get_command(user_input: str, image: Image) -> Image:
    """ Author: TeamMate
    Returns the image transformations when the user provides input commands and a loaded image. """
    
    filters = {"3":three_tone(image, "blood", "lemon", "gray") , "X":extreme_contrast(image) , "P":posterize(image) , "V":flip_vertical(image) , "H":flip_horizontal(image), "T":sepia_channel(image)}
    image = filters.get(user_input) 
    return image
    
    
def run_interface() -> Image:
    """ Authors: TeamMate and TeamMate
    Executes the user interface so that the user may view a list of commands and input a given series of commands in order to create an image. """
    
    commands = ["S", "E", "Q", "3", "X", "T", "P", "L", "D", "V", "H"] 
    image = None
    user_input = user_command()
    
    while user_input != "Q":
        if user_input not in commands:
            print("No such command")
        elif user_input == "L":
            image = load_image(choose_file())
            show(image)
        elif image == None:
            print("No image loaded")
        else:
            if user_input == "E":
                threshold = int(input("Enter the threshold value: "))
                image = detect_edges(image, threshold) 
            elif user_input == "D":
                curve_drawing = draw_curve(image, "cyan", None)
                print(curve_drawing[1])
                image = curve_drawing[0]              
            elif user_input == "S":
                file_name = str(input("Save image as (include file type): "))
                save_as(image, file_name)
            else:
                image = get_command(user_input, image)
            show(image)
               
        user_input = user_command()
    
    return image
    
    
# Main script        
image = run_interface()

