import os
from termcolor import colored
import subprocess
import sys

os.system("create.py")
print(colored("the analysis of the working hours has been completed!", 'blue'))
print("Запуск программы...\n")
os.system("algorithm/cmake-build-debug/ScheduleGenerator.exe")
print("Завершено!")
