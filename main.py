import os
from termcolor import colored
import subprocess
import sys

os.system("python create.py")
print(colored("the analysis of the working hours has been completed!", 'blue'))
print("Запуск программы...\n")
# os.system("./algorithm/main.exe")
print("Визуализация...\n")
os.system('python front/builder_for_classes.py')
os.system('python front/builder_for_teachers.py')