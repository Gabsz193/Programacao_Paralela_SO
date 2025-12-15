import subprocess
import os
import csv
import matplotlib.pyplot as plt

# Configuration
SIZES = [10, 50, 100, 500]
THREADS = [1, 2, 4, 8]
OUTPUT_FILE = "out/sim-benchmark.csv"
EXECUTABLE = "./build/simulation"

def run_benchmark():
    results = []

    # Configura o SDL para usar um dispositivo falso como alvo da renderização
    env = os.environ.copy()
    env['SDL_VIDEODRIVER'] = 'dummy'

    print(f"Running benchmarks with sizes: {SIZES} and threads: {THREADS}")
    
    # Sequential
    for size in SIZES:
        print(f"Running Sequential Size={size}")
        cmd = [EXECUTABLE, "--size", str(size), "--mode", "seq"]
        result = subprocess.run(cmd, capture_output=True, text=True, env=env)
        if result.returncode != 0:
            print(f"Error: {result.stderr}")
            continue

        # Output format: mode,size,threads,fps
        parts = result.stdout.strip().split(',')
        results.append({"mode": "seq", "size": size, "threads": 1, "fps": float(parts[3])})

    # Parallel
    for size in SIZES:
        for t in THREADS:
            print(f"Running Parallel Size={size} Threads={t}")
            cmd = [EXECUTABLE, "--size", str(size), "--mode", "par", "--threads", str(t)]
            result = subprocess.run(cmd, check=True, capture_output=True, text=True, env=env)
            if result.returncode != 0:
                print(f"Error: {result.stderr}")
                continue
            
            # Output format: mode,size,threads,fps
            parts = result.stdout.strip().split(',')
            results.append({"mode": "par", "size": size, "threads": t, "fps": float(parts[3])})
            
    return results

def save_results(results):
    with open(OUTPUT_FILE, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=["mode", "size", "threads", "fps"])
        writer.writeheader()
        writer.writerows(results)

def generate_graphs(results):
    # Organize data
    seq_times = {r['size']: r['fps'] for r in results if r['mode'] == 'seq'}

    # Execution Time Graph
    plt.figure(figsize=(10, 6))
    
    # Plot Sequential
    sizes = sorted(list(seq_times.keys()))
    times = [seq_times[s] for s in sizes]
    plt.plot(sizes, times, marker='x', linestyle='--', label='Sequencial', color='black')

    for t in THREADS:
        times = []
        current_sizes = []
        for size in SIZES:
            par_time = next((r['fps'] for r in results if r['mode'] == 'par' and r['size'] == size and r['threads'] == t), None)
            if par_time:
                times.append(par_time)
                current_sizes.append(size)
        plt.plot(current_sizes, times, marker='o', label=f'{t} Threads')

    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Quantidade de Bolinhas')
    plt.ylabel('FPS (quadros/s)')
    plt.title('FPS vs Quantidade de Bolinhas')
    plt.legend()
    plt.grid(True)
    plt.savefig('out/sim-execution_time.png')
    print("Generated sim-execution_time.png")

def print_table(results):
    print("\nBenchmark Results:")
    print(f"{'Mode':<10} {'Size':<15} {'Threads':<10} {'FPS (frames/s)':<15} {'Speedup':<10}")
    print("-" * 60)
    
    seq_times = {r['size']: r['fps'] for r in results if r['mode'] == 'seq'}
    
    for r in results:
        speedup = 1.0
        if r['mode'] == 'par':
            speedup = seq_times[r['size']] / r['fps']
        
        print(f"{r['mode']:<10} {r['size']:<15} {r['threads']:<10} {r['fps']:<15.6f} {speedup:<10.2f}")

if __name__ == "__main__":
    print("Running benchmarks...")
    try:
        results = run_benchmark()
        save_results(results)
        print_table(results)
        generate_graphs(results)
    except Exception as e:
        print(f"An error occurred: {e}")
