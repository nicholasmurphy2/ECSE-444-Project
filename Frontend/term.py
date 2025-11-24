import serial
import pygame
import json


COM_PORT = "COM6"
PRINT_ALL_MESSAGES = False

ser = None
ser_retry_time = 0

pygame.init()
pygame.font.init()

font_main = pygame.font.Font("W95FA.otf", 14)
font_underline = pygame.font.Font("W95FA.otf", 14)
font_underline.set_underline(True)
font_large = pygame.font.Font("W95FA.otf", 28)

background = pygame.image.load("gui.png")
low_power_bg = pygame.image.load("dither_it_pipes.png")
score_img = pygame.image.load("score_window_tight.png")
timer_img = pygame.image.load("timer_window_tight.png")
bop_it_img = pygame.image.load("bop_it.png")
clap_it_img = pygame.image.load("clap_it.png")
cover_it_img = pygame.image.load("cover_it.png")

screen = pygame.display.set_mode([1025, 768])
running = True

score = 0
app_mode = "idle"
command = ""
low_power = False
time_left = 0
counter = 0

dt = 0
clock = pygame.Clock()
clock.tick()

while running:
    dt = clock.tick()

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    if ser is None:
        ser_retry_time -= dt
        if ser_retry_time < 0:
            try:
                ser = serial.Serial(COM_PORT, baudrate=115200)
                counter = 0
            except serial.serialutil.SerialException:
                print(f"Could not connect to {COM_PORT} serial port. Retrying...")
                ser_retry_time = 2000

    if low_power:
        screen.blit(low_power_bg, (0, 0))

    else:
        screen.blit(background, (0, 0))

        # app mode
        app_mode_text = font_underline.render(app_mode, False, (0, 0, 0))
        screen.blit(app_mode_text, (150, 56))

        # command
        if app_mode == "playing_command":
            command_text = font_large.render(command, False, (0, 0, 0))
            screen.blit(command_text, (150, 100))
            pos = (200, 150)
            if command == "bop_it":
                screen.blit(bop_it_img, pos)
            elif command == "clap_it":
                screen.blit(clap_it_img, pos)
            elif command == "cover_it":
                screen.blit(cover_it_img, pos)

        # score
        if app_mode != "idle":
            screen.blit(score_img, (590, 555))
            score_text = font_large.render(str(score), False, (0, 0, 0))
            screen.blit(score_text, (675, 585))

        # timer
        if time_left > 0:
            time_left_s = time_left // 1000
            time_left_ds = (time_left % 1000) // 100
            screen.blit(timer_img, (700, 200))
            time_left_text = font_large.render(
                f"{time_left_s}.{time_left_ds} s", False, (0, 0, 0)
            )
            screen.blit(time_left_text, (785, 230))

        # counter/status
        counter_str = str(counter) if ser is not None else "DISCON."
        counter_text = font_main.render(counter_str, False, (0, 0, 0))
        screen.blit(counter_text, (970, 751))

    collecting_messages = True
    while collecting_messages:
        in_waiting = 0
        if ser is not None:
            try:
                in_waiting = ser.in_waiting
            except:
                ser = None
        if in_waiting == 0:
            collecting_messages = False
            continue

        data = None
        try:
            line = ser.readline()
            s = line.decode("ascii")
            if PRINT_ALL_MESSAGES:
                print(s)
            if s.startswith("{"):
                data = json.loads(s)
        except Exception as e:
            print(e)
            print(line)

        if data is not None:
            counter += 1

            if "app_mode" in data:
                app_mode = data["app_mode"]
            if "score" in data:
                score = data["score"]
            if "command" in data:
                command = data["command"]
            if "low_power" in data:
                low_power = data["low_power"]
            if "time_left" in data:
                time_left = data["time_left"]

    pygame.display.flip()


pygame.quit()
