import os
import subprocess
import sys
import time
from pathlib import Path
from typing import Optional, List, Tuple
import pickle
import zlib
import base64
import hashlib

# Text Color Constants
COLOR_RESET = "\033[0m"
COLOR_GREEN = "\033[1;32m"  # Bright Green
COLOR_RED = "\033[1;31m"    # Bright Red
COLOR_YELLOW = "\033[1;33m" # Bright Yellow
COLOR_BLUE = "\033[1;34m"   # Bright Blue

# Progress Bar Gradient
def color_gradient(percentage: float) -> str:
    """
    Returns a color that transitions smoothly from red to green based on the percentage.
    """
    red = int((1 - percentage) * 255)
    green = int(percentage * 255)
    return f"\033[38;2;{red};{green};0m"

DEFAULT_RUN_COUNT = 100
TEST_SERVER = "planet.cs.biu.ac.il"

RUN_COUNT = int(sys.argv[1]) if len(sys.argv) == 2 else DEFAULT_RUN_COUNT
HOST = subprocess.check_output("hostname").decode().strip()

USERNAME = subprocess.check_output("whoami").decode().strip()
USERNAME = hashlib.md5(USERNAME.encode()).hexdigest()

TEMP_FILE_PATH = Path(".compare_tmp")

MATRICES = "eJx1VUFOw0AMDBQEsQQFXhBx6gmt+cZ+gAvHPIEjEo8o7+FrtGmSnRk7jZJGznrWM/YkPze/f303/b6Ph4/XXde9FyvD+XC4nv/1KOZrvGVgbF5pDs8wwxnZ4q7L84I5hvlOyJRpuiLLWvZljnw/rzdUY4tJATzVRCLWYhki8mUdPCD6zJf1L7Ji4esDa6KaO+nstDdzRZ1FhVBr21cVkSwrQwkI2KH53jRbV/lan1anvS5rf7Vujk2Zx8/joV7V23pX7+tDfaz7+lSfx8U9yl9Rm7rqM0/qW7oa+y4Rc8JSXXBdPmuSb5lGsbONR3QHXcUVcfJLwNvkCnyzdwnlmcYRn6ddFYvoBaZE5120prdF9Cy7UdmqNxveRlU0L5RHJ1xNmWK/AdM8WRP7jG8fdRBN8OSebvZPf/FOfRnbt4enUbq+7GlaRz6lxRRrw23G3OM7R/HYZzJL4tpcmYtq/JynE/Yx3kG1XXGlCzlndnfEhAzLVNFZceg+Ozecxhx1ShsP0Un2W6uhfmgFgGktJ+txc2PSc6jSYZ1GOYL1pZ1idqLzZp9X91zX3enr01c7fYH249fbP89Mibg="
MATRICES = base64.b64decode(MATRICES)
MATRICES = zlib.decompress(MATRICES)
MATRICES = pickle.loads(MATRICES)


RESULTS = {i: [] for i in range(len(MATRICES))}

PARAMS_TO_FORM_FIELDS = {
    "0": "entry.295728809",
    "1": "entry.266687381",
    "2": "entry.296772969",
    "USER": "entry.1014437322"
}


def print_color(msg: str, color: str = COLOR_RESET, end="\n"):
    """
    Prints a message in the specified color.
    """
    sys.stdout.buffer.write(str_colored(msg, color, end))

def str_colored(msg: str, color: str = COLOR_RESET, end="\n") -> bytes:
    """
    Returns a message in the specified color.
    """
    return f"{color}{msg}{COLOR_RESET}{end}".encode()


def run_program(matrix) -> Optional[Tuple[float, List[int]]]:
    matrix = matrix.replace("\n", "\\n")
    os.system(f'printf "{matrix}" | ./clique > {TEMP_FILE_PATH} 2>/dev/null')
    output = TEMP_FILE_PATH.read_text()
    if TEMP_FILE_PATH.exists():
        TEMP_FILE_PATH.unlink()
    try:
        lines = output.splitlines()
        runtime = float(lines[-1].split()[-1])  # Message is of format: Time spent: 0.0639370000
        clique_line = lines[-3].replace("Clique Members: ", "")
        clique_members = list(map(int, clique_line.split()))
        return runtime, clique_members
    except Exception:
        return None


def show_progress(current, total, start_time, bar_length=40):
    """
    Displays a flat-style progress bar with a color gradient and items-per-second indicator.
    """
    elapsed_time = time.time() - start_time
    items_per_second = current / elapsed_time if elapsed_time > 0 else 0
    progress = current / total
    filled = int(bar_length * progress)
    empty = bar_length - filled
    bar_color = color_gradient(progress)
    bar = f"{bar_color}{'█' * filled}{'░' * empty}{COLOR_RESET}"
    percentage = f"{progress * 100:.1f}%"
    sys.stdout.write(
        f"\r{bar} {current}/{total} {percentage} ({items_per_second:.2f} items/s)"
    )
    sys.stdout.flush()

def main():
    ERROR = None
    sys.stdout.buffer.write("Compiling...".encode())
    try:
        subprocess.check_call("rm -f clique ; gcc -O0 -o clique main.c max-clique.c", shell=True)
        print_color(" Success", COLOR_GREEN)
    except:
        print_color(" Failed", COLOR_RED)
        return

    print()

    print_color(f"Running test {RUN_COUNT} times...", COLOR_BLUE)

    start_time = time.time()
    for iteration in range(RUN_COUNT):
        if ERROR:
            break

        show_progress(iteration + 1, RUN_COUNT, start_time)
        for i, (matrix, expected_vertices) in enumerate(MATRICES.items()):
            time_took, clique_members = run_program(matrix) or (None, None)
            if time_took is None:
                ERROR = str_colored("\n❌ Failed (Error while running)", COLOR_RED)
                break
            else:
                if clique_members == expected_vertices:
                    RESULTS[i].append(time_took)
                else:
                    ERROR = str_colored(f"\n❌ Failed (Output: {clique_members} is not clique)", COLOR_RED)
                    break

    if ERROR:
        sys.stdout.buffer.write(ERROR)
    else:
        elapsed_time = time.time() - start_time
        print_color(f"\nCompleted in {elapsed_time:.2f} seconds!\n", COLOR_GREEN)

        print_color("  Runtimes  ".center(100, "="), COLOR_BLUE)
        for i, timings in RESULTS.items():
            if timings:
                average = sum(timings) / len(timings)
                print_color(f"Test {i:<30} Average runtime: {average:.6f} ms", COLOR_GREEN)

    if HOST != TEST_SERVER:
        print_color(f"❗ Not uploading results because not running on {TEST_SERVER}", COLOR_YELLOW)
    elif ERROR:
        print_color("❗ Not uploading results because not all runs were successful", COLOR_RED)
    elif RUN_COUNT != 1000:
        print_color("❗ Not uploading results because the average wasn't on 1000 runs", COLOR_YELLOW)
    else:
        print_color("📤 Uploading results to Google Forms...", COLOR_GREEN)

        entries_data = []
        for i, timings in RESULTS.items():
            average = sum(timings) / len(timings)
            entries_data.append(f"{PARAMS_TO_FORM_FIELDS[str(i)]}={average:.6f}")

        entries_data.append(f"{PARAMS_TO_FORM_FIELDS['USER']}={USERNAME}")

        os.system(f"""
        curl 'https://docs.google.com/forms/d/e/1FAIpQLScIN4njh40n6osL4Ag4qJf5RsS88_q6NqEHYwWJeI-Wn1seJw/formResponse' \
          -H 'authority: docs.google.com' \
          -H 'accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9' \
          -H 'accept-language: en-GB,en;q=0.9' \
          -H 'cache-control: max-age=0' \
          -H 'content-type: application/x-www-form-urlencoded' \
          -H 'origin: https://docs.google.com' \
          -H 'referer: https://docs.google.com/forms/d/e/1FAIpQLScIN4njh40n6osL4Ag4qJf5RsS88_q6NqEHYwWJeI-Wn1seJw/viewform?fbzx=4387170321690009457' \
          -H 'sec-ch-ua: ".Not/A)Brand";v="99", "Google Chrome";v="103", "Chromium";v="103"' \
          -H 'sec-ch-ua-arch: "x86"' \
          -H 'sec-ch-ua-bitness: "64"' \
          -H 'sec-ch-ua-full-version-list: ".Not/A)Brand";v="99.0.0.0", "Google Chrome";v="103.0.5060.53", "Chromium";v="103.0.5060.53"' \
          -H 'sec-ch-ua-mobile: ?0' \
          -H 'sec-ch-ua-model: ""' \
          -H 'sec-ch-ua-platform: "Linux"' \
          -H 'sec-ch-ua-platform-version: "5.15.0"' \
          -H 'sec-ch-ua-wow64: ?0' \
          -H 'sec-fetch-dest: document' \
          -H 'sec-fetch-mode: navigate' \
          -H 'sec-fetch-site: same-origin' \
          -H 'sec-fetch-user: ?1' \
          -H 'upgrade-insecure-requests: 1' \
          -H 'user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.0.0 Safari/537.36' \
          --data "{'&'.join(entries_data)}&fvv=1&partialResponse=%5Bnull%2Cnull%2C%224387170321690009457%22%5D&pageHistory=0&fbzx=4387170321690009457" \
              --compressed > /dev/null 2>&1
            """)


        print_color("View others' results at: https://docs.google.com/spreadsheets/d/1Dq6T_zcLeEfznZ_hyaiy3hMaljXtM9XUwCmvM9xhsxg/edit?resourcekey=&gid=1730012880#gid=1730012880", COLOR_BLUE)
        print_color(f"Your user ID is {USERNAME}", COLOR_BLUE)

    print_color("Test script written by Ron Even", COLOR_BLUE)
    print_color("May the odds be ever in your favor!", COLOR_GREEN)


if __name__ == "__main__":
    main()
