import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

from allcode3new import FractalTensor, Evolver, Extender, FractalKnowledgeBase, Transcender
from Test2.testletras import corpus, build_letter_trios

# Demo: visualiza los tensores de las sílabas del corpus como puntos en 3D


def extraer_puntos_fractales(corpus):
    trios = build_letter_trios(corpus)
    puntos = []
    for trio in trios:
        for tensor in trio:
            v = tensor.nivel_3[0]
            puntos.append(v)
    return np.array(puntos)

# --- Generador de tensores sintéticos con patrón de espiral natural (tipo filotaxia) ---

def generar_tensores_espiral(n=100, a=1.0, b=0.2):
    """Genera tensores FractalTensor siguiendo una espiral 3D tipo filotaxia."""
    phi = np.pi * (3 - np.sqrt(5))  # ángulo áureo
    puntos = []
    tensores = []
    for i in range(n):
        y = 1 - (i / float(n - 1)) * 2  # de 1 a -1
        radius = np.sqrt(1 - y * y)
        theta = phi * i
        x = np.cos(theta) * radius
        z = np.sin(theta) * radius
        # Escalamos y discretizamos para simular ternarios
        v = np.array([int(round((x + 1) * a)), int(round((y + 1) * a)), int(round((z + 1) * a))], dtype=float)
        puntos.append(v)
        tensores.append(FractalTensor(nivel_3=[v, [0,0,0], [0,0,0]]))
    return tensores, np.array(puntos)

def plot_fractal_points(puntos, puntos_sinteticos=None, colores_sint=None, tray=None):
    """Visualiza puntos reales, sintéticos y trayectoria en 3D."""
    fig = plt.figure(figsize=(8, 6), facecolor='black')
    ax = fig.add_subplot(111, projection='3d', facecolor='black')
    # Puntos reales
    if puntos is not None and len(puntos) > 0:
        puntos = np.array(puntos)
        ax.scatter(puntos[:,0], puntos[:,1], puntos[:,2], s=60, c='white', alpha=0.8, label='Reales')
    # Puntos sintéticos
    if puntos_sinteticos is not None and len(puntos_sinteticos) > 0:
        c = colores_sint if colores_sint is not None else 'lime'
        ax.scatter(puntos_sinteticos[:,0], puntos_sinteticos[:,1], puntos_sinteticos[:,2], s=30, c=c, alpha=0.7, label='Sintéticos')
    # Trayectoria
    if tray is not None and len(tray) > 0:
        tray = np.array(tray)
        if tray.ndim == 2 and tray.shape[1] == 3:
            ax.plot(tray[:,0], tray[:,1], tray[:,2], color='cyan', linewidth=2, label='Dinámica Extender')
            ax.scatter(tray[:,0], tray[:,1], tray[:,2], s=10, c='cyan', alpha=0.7)
        elif tray.ndim == 1 and tray.shape[0] == 3:
            ax.scatter([tray[0]], [tray[1]], [tray[2]], s=20, c='cyan', alpha=0.7)
    ax.set_axis_off()
    plt.tight_layout()
    plt.show()


def plot_fractal_points(puntos, puntos_sinteticos=None, colores_sint=None, tray=None):
    """Visualiza puntos reales, sintéticos y trayectoria en 3D."""
    fig = plt.figure(figsize=(8, 6), facecolor='black')
    ax = fig.add_subplot(111, projection='3d', facecolor='black')
    # Puntos reales
    if puntos is not None and len(puntos) > 0:
        puntos = np.array(puntos)
        ax.scatter(puntos[:,0], puntos[:,1], puntos[:,2], s=60, c='white', alpha=0.8, label='Reales')
    # Puntos sintéticos
    if puntos_sinteticos is not None and len(puntos_sinteticos) > 0:
        c = colores_sint if colores_sint is not None else 'lime'
        ax.scatter(puntos_sinteticos[:,0], puntos_sinteticos[:,1], puntos_sinteticos[:,2], s=30, c=c, alpha=0.7, label='Sintéticos')
    # Trayectoria
    if tray is not None and len(tray) > 0:
        tray = np.array(tray)
        if tray.ndim == 2 and tray.shape[1] == 3:
            ax.plot(tray[:,0], tray[:,1], tray[:,2], color='cyan', linewidth=2, label='Dinámica Extender')
            ax.scatter(tray[:,0], tray[:,1], tray[:,2], s=10, c='cyan', alpha=0.7)
        elif tray.ndim == 1 and tray.shape[0] == 3:
            ax.scatter([tray[0]], [tray[1]], [tray[2]], s=20, c='cyan', alpha=0.7)
    ax.set_axis_off()
    plt.tight_layout()
    plt.show()

def animar_trayectoria(tray, puntos, puntos_sinteticos=None, colores_sint=None):
    import matplotlib.animation as animation
    tray = np.array(tray)
    fig = plt.figure(figsize=(8, 6), facecolor='black')
    ax = fig.add_subplot(111, projection='3d', facecolor='black')
    ax.scatter(puntos[:,0], puntos[:,1], puntos[:,2], s=60, c='white', alpha=0.8, label='Reales')
    if puntos_sinteticos is not None:
        c = colores_sint if colores_sint is not None else 'lime'
        ax.scatter(puntos_sinteticos[:,0], puntos_sinteticos[:,1], puntos_sinteticos[:,2], s=30, c=c, alpha=0.7, label='Sintéticos')
    scat, = ax.plot([], [], [], color='cyan', linewidth=2)
    ax.set_axis_off()
    plt.tight_layout()

    def update(num):
        if num < 2:
            scat.set_data([], [])
            scat.set_3d_properties([])
        else:
            scat.set_data(tray[:num,0], tray[:num,1])
            scat.set_3d_properties(tray[:num,2])
        return scat,

    ani = animation.FuncAnimation(fig, update, frames=len(tray), interval=40, blit=True)
    plt.show()




if __name__ == "__main__":
    puntos = extraer_puntos_fractales(corpus)
    tensores_sint, puntos_sint = generar_tensores_espiral(n=120, a=2.5)
    evolver = Evolver()
    transcender = Transcender()
    # Usamos analyze_fractal_dynamics sobre trios consecutivos
    valores = []
    for i in range(len(tensores_sint)-2):
        trio = [tensores_sint[i], tensores_sint[i+1], tensores_sint[i+2]]
        try:
            res = evolver.analyze_fractal_dynamics(trio)
            if isinstance(res, dict) and 'M_emergent' in res:
                val = sum(res['M_emergent'])
            else:
                val = 0
        except Exception:
            val = 0
        valores.append(val)
    valores = np.array(valores)
    if len(valores) > 0:
        valores = (valores - valores.min()) / (np.ptp(valores) + 1e-8)
        colores = plt.cm.plasma(valores)
        puntos_sint_plot = puntos_sint[:len(valores)]
    else:
        colores = 'lime'
        puntos_sint_plot = puntos_sint

    # --- Ciclo completo: Transcender -> Extender -> Evolver (animable) ---
    kb = FractalKnowledgeBase()
    extender = Extender(knowledge_base=kb)
    tray = []
    estado = tensores_sint[0]
    for i in range(1, len(tensores_sint)-2):
        # 1. Transcender: síntesis fractal de 3 tensores
        t1, t2, t3 = tensores_sint[i-1], tensores_sint[i], tensores_sint[i+1]
        try:
            synth = transcender.compute_full_fractal(t1, t2, t3)
        except Exception:
            synth = estado
        # 2. Extender: extensión fractal
        try:
            out = extender.extend_fractal(synth.nivel_3[0], {'space_id': 'sintetico'})
            if 'reconstructed_tensor' in out:
                v = out['reconstructed_tensor']
                # Convertir a vector numpy si es necesario
                if isinstance(v, (list, tuple)) and len(v) == 3:
                    v = np.array(v, dtype=float)
                    tray.append(v)
                    estado = FractalTensor(nivel_3=[v, [0,0,0], [0,0,0]])
                elif isinstance(v, np.ndarray) and v.shape == (3,):
                    tray.append(v)
                    estado = FractalTensor(nivel_3=[v, [0,0,0], [0,0,0]])
        except Exception:
            continue
    # Visualización estática
    plot_fractal_points(puntos, puntos_sinteticos=puntos_sint_plot, colores_sint=colores, tray=tray)
    # Animación de la trayectoria
    if len(tray) > 2:
        animar_trayectoria(tray, puntos, puntos_sinteticos=puntos_sint_plot, colores_sint=colores)
