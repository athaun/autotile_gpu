import os
import re
import sys
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

input_file = "perf_report_no_set.log"

output_folder = os.path.splitext(input_file)[0]
os.makedirs(output_folder, exist_ok=True)

with open(input_file, "r") as file:
    data = file.read()

# Regular expressions to parse the log
pattern = re.compile(
    r"apply_deltas: ([\d.]+) ms \(([\d.]+)%\)\n"
    r"send_deltas: ([\d.]+) ms \(([\d.]+)%\)\n"
    r"choose_delta: ([\d.]+) ms \(([\d.]+)%\)\n"
    r"check_attachment: ([\d.]+) ms \(([\d.]+)%\)\n"
    r"check_transitions: ([\d.]+) ms \(([\d.]+)%\)\n"
    r"choose_location: ([\d.]+) ms \(([\d.]+)%\)\n"
    r"Ticks: (\d+)\n"
    r"Grid Size: (\d+)x(\d+)\n"
    r"Available Locations: (\d+)\n"
    r"Delta Buffer Count: (\d+)"
)

# Parse log entries
entries = []
for match in pattern.finditer(data):
    entries.append({
        "apply_deltas_ms": float(match.group(1)),
        "send_deltas_ms": float(match.group(3)),
        "choose_delta_ms": float(match.group(5)),
        "check_attachment_ms": float(match.group(7)),
        "check_transitions_ms": float(match.group(9)),
        "choose_location_ms": float(match.group(11)),
        "apply_deltas_pct": float(match.group(2)),
        "send_deltas_pct": float(match.group(4)),
        "choose_delta_pct": float(match.group(6)),
        "check_attachment_pct": float(match.group(8)),
        "check_transitions_pct": float(match.group(10)),
        "choose_location_pct": float(match.group(12)),
        "ticks": int(match.group(13)),
        "grid_width": int(match.group(14)),
        "grid_height": int(match.group(15)),
        "available_locations": int(match.group(16)),
        "delta_buffer_count": int(match.group(17)),
    })

# Convert to DataFrame
df = pd.DataFrame(entries)

# Plot 1: Execution time vs Ticks
plt.figure(figsize=(12, 8))
for col in ["apply_deltas_ms", "send_deltas_ms", "choose_delta_ms",
            "check_attachment_ms", "check_transitions_ms", "choose_location_ms"]:
    sns.lineplot(x="ticks", y=col, data=df, label=col.replace("_ms", ""))
plt.title("Execution Time vs Ticks")
plt.xlabel("Ticks")
plt.ylabel("Time (ms)")
plt.legend()
plt.savefig(f"{output_folder}/execution_time_vs_ticks.png")
plt.close()

# Plot 2: Percentage breakdown of tasks over time
plt.figure(figsize=(12, 8))
for col in ["apply_deltas_pct", "send_deltas_pct", "choose_delta_pct",
            "check_attachment_pct", "check_transitions_pct", "choose_location_pct"]:
    sns.lineplot(x="ticks", y=col, data=df, label=col.replace("_pct", ""))
plt.title("Percentage Breakdown of Task Durations Over Time")
plt.xlabel("Ticks")
plt.ylabel("Percentage (%)")
plt.legend()
plt.savefig(f"{output_folder}/percentage_breakdown_vs_ticks.png")
plt.close()

# Plot 4: Grid Size Evolution
plt.figure(figsize=(12, 8))
sns.lineplot(x="ticks", y="grid_width", data=df, label="Grid Width")
sns.lineplot(x="ticks", y="grid_height", data=df, label="Grid Height")
plt.title("Grid Size Evolution")
plt.xlabel("Ticks")
plt.ylabel("Grid Size")
plt.legend()
plt.savefig(f"{output_folder}/grid_size_vs_ticks.png")
plt.close()

print(f"Plots saved in '{output_folder}' directory.")
