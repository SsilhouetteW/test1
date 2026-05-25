#!/usr/bin/env python3
"""
STM32 Sensor Dashboard
订阅 MQTT 主题显示温度和光照强度数据
需要先安装: pip install paho-mqtt pystray pillow
"""

import tkinter as tk
from tkinter import font as tkfont
import paho.mqtt.client as mqtt
from datetime import datetime
import threading
import time

# MQTT 配置
MQTT_BROKER = "broker.emqx.io"
MQTT_PORT = 1883
MQTT_TOPIC_TEMP = "sensor/temperature"
MQTT_TOPIC_LIGHT = "sensor/light"

# 全局变量
temp_value = "--.-"
light_value = "----"
last_update_temp = None
last_update_light = None

# 窗口
root = None
temp_label = None
light_label = None
time_label = None
log_text = None


def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        log(f"[MQTT] Connected to {MQTT_BROKER}")
        client.subscribe(MQTT_TOPIC_TEMP)
        client.subscribe(MQTT_TOPIC_LIGHT)
        log(f"[MQTT] Subscribed: {MQTT_TOPIC_TEMP}, {MQTT_TOPIC_LIGHT}")
    else:
        log(f"[MQTT] Connection failed, code: {rc}")


def on_message(client, userdata, msg):
    global temp_value, light_value, last_update_temp, last_update_light
    
    payload = msg.payload.decode()
    topic = msg.topic
    
    if topic == MQTT_TOPIC_TEMP:
        temp_value = payload
        last_update_temp = datetime.now()
        log(f"[TEMP] {payload} C")
        update_display()
    elif topic == MQTT_TOPIC_LIGHT:
        light_value = payload
        last_update_light = datetime.now()
        log(f"[LIGHT] {payload}")
        update_display()


def on_disconnect(client, userdata, rc, properties=None):
    log(f"[MQTT] Disconnected, code: {rc}")
    reconnect_thread = threading.Thread(target=reconnect_mqtt, daemon=True)
    reconnect_thread.start()


def reconnect_mqtt():
    time.sleep(3)
    log("[MQTT] Reconnecting...")
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
    except Exception as e:
        log(f"[ERROR] Reconnect failed: {e}")


def log(message):
    if log_text:
        timestamp = datetime.now().strftime("%H:%M:%S")
        log_text.insert(tk.END, f"[{timestamp}] {message}\n")
        log_text.see(tk.END)


def update_display():
    global temp_label, light_label, time_label
    
    if temp_label:
        temp_label.config(text=f"{temp_value} C")
    
    if light_label:
        light_label.config(text=f"{light_value}")
    
    if time_label:
        now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        time_label.config(text=f"Last Update: {now}")


def create_gui():
    global root, temp_label, light_label, time_label, log_text
    
    root = tk.Tk()
    root.title("STM32 Sensor Dashboard")
    root.geometry("480x400")
    root.resizable(True, True)
    
    # 标题字体
    title_font = tkfont.Font(family="Arial", size=24, weight="bold")
    value_font = tkfont.Font(family="Arial", size=48, weight="bold")
    label_font = tkfont.Font(family="Arial", size=14)
    small_font = tkfont.Font(family="Arial", size=10)
    
    # 主框架
    main_frame = tk.Frame(root, bg="#2b2b2b")
    main_frame.pack(fill=tk.BOTH, expand=True)
    
    # 标题
    title_label = tk.Label(main_frame, text="STM32 Sensor Monitor",
                           font=title_font, fg="white", bg="#2b2b2b")
    title_label.pack(pady=20)
    
    # 数据显示框架
    data_frame = tk.Frame(main_frame, bg="#2b2b2b")
    data_frame.pack(pady=10)
    
    # 温度卡片
    temp_card = tk.Frame(data_frame, bg="#3c3c3c", padx=30, pady=20)
    temp_card.grid(row=0, column=0, padx=15)
    
    tk.Label(temp_card, text="Temperature", font=label_font,
             fg="#888", bg="#3c3c3c").pack()
    temp_label = tk.Label(temp_card, text="--.- C", font=value_font,
                          fg="#2196F3", bg="#3c3c3c")
    temp_label.pack()
    tk.Label(temp_card, text="(Internal Sensor)", font=small_font,
             fg="#666", bg="#3c3c3c").pack()
    
    # 光照卡片
    light_card = tk.Frame(data_frame, bg="#3c3c3c", padx=30, pady=20)
    light_card.grid(row=0, column=1, padx=15)
    
    tk.Label(light_card, text="Light Intensity", font=label_font,
             fg="#888", bg="#3c3c3c").pack()
    light_label = tk.Label(light_card, text="----", font=value_font,
                           fg="#FF9800", bg="#3c3c3c")
    light_label.pack()
    tk.Label(light_card, text="(ADC Channel 1)", font=small_font,
             fg="#666", bg="#3c3c3c").pack()
    
    # 时间标签
    time_label = tk.Label(main_frame, text="Waiting for data...",
                          font=small_font, fg="#666", bg="#2b2b2b")
    time_label.pack(pady=5)
    
    # 日志框架
    log_frame = tk.Frame(main_frame, bg="#1a1a1a")
    log_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=10)
    
    tk.Label(log_frame, text="MQTT Log", font=label_font,
             fg="#888", bg="#1a1a1a").pack(anchor=tk.W)
    
    log_text = tk.Text(log_frame, height=8, bg="#1a1a1a",
                       fg="#4caf50", font=("Consolas", 9),
                       relief=tk.FLAT, state=tk.NORMAL)
    log_text.pack(fill=tk.BOTH, expand=True, pady=5)
    
    # 状态栏
    status_bar = tk.Label(main_frame, text="MQTT: Connecting...",
                           font=small_font, fg="#888", bg="#2b2b2b")
    status_bar.pack(side=tk.BOTTOM, pady=5)
    
    def update_status():
        status_bar.config(text=f"MQTT: {MQTT_BROKER}:{MQTT_PORT} | Topics: {MQTT_TOPIC_TEMP}, {MQTT_TOPIC_LIGHT}")
    
    update_status()
    
    root.mainloop()


# MQTT 客户端
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect


if __name__ == "__main__":
    print("=" * 50)
    print("  STM32 Sensor Dashboard")
    print("=" * 50)
    print(f"  Broker: {MQTT_BROKER}:{MQTT_PORT}")
    print(f"  Topics: {MQTT_TOPIC_TEMP}, {MQTT_TOPIC_LIGHT}")
    print("=" * 50)
    print()
    
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
        client.loop_start()
        print("[OK] MQTT client started")
        print("[OK] GUI window opening...")
        print()
    except Exception as e:
        print(f"[ERROR] Failed to connect to MQTT: {e}")
        print("[INFO] Please check your internet connection")
    
    create_gui()
