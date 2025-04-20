import os
from termcolor import colored
import subprocess
import sys

os.system("python create.py")
print(colored("the analysis of the working hours has been completed!", 'blue'))
# print(colored("RUN================================\n",'yellow'))
# os.system(".\\algorithm\\main.exe")
# print(colored("DONE===============================\n",'yellow'))
os.system('python front/builder_for_classes.py')
os.system('python front/builder_for_teachers.py')
