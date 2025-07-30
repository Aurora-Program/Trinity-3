import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Generador de "ramas" fractales tipo pluma/filamento

def generar_filamento(num_puntos=2000, ramificaciones=7, semilla=42):
    np.random.seed(semilla)
    t = np.linspace(0, 12 * np.pi, num_puntos)
    x = np.sin(t) * (1 + 0.15 * np.random.randn(num_puntos))
    y = np.cos(t) * (1 + 0.15 * np.random.randn(num_puntos))
    z = t / (2 * np.pi) + 0.1 * np.random.randn(num_puntos)
    puntos = [(x, y, z)]
    # Ramificaciones laterales
    for i in range(1, ramificaciones+1):
        fase = np.pi * 2 * i / ramificaciones
        x2 = x + 0.25 * np.sin(t*2 + fase) * (1 + 0.2 * np.random.randn(num_puntos))
        y2 = y + 0.25 * np.cos(t*2 + fase) * (1 + 0.2 * np.random.randn(num_puntos))
        z2 = z + 0.1 * np.sin(t*3 + fase)
        puntos.append((x2, y2, z2))
    return puntos

def plot_filamento(puntos):
    fig = plt.figure(figsize=(10, 6), facecolor='black')
    ax = fig.add_subplot(111, projection='3d', facecolor='black')
    for x, y, z in puntos:
        ax.scatter(x, y, z, s=0.5, c='white', alpha=0.8)
    ax.set_axis_off()
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    puntos = generar_filamento(num_puntos=3000, ramificaciones=12)
    plot_filamento(puntos)
