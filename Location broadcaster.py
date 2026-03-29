import socket
import struct
import tkinter as tk

# Configuration
PORT = 5555
# UPDATED: 60 bytes (4-byte ID + 4-byte padding + 40 bytes doubles + 8 bytes uint64)
ADSB_STRUCT_FMT = "<I 4x d d d d d Q"

class RadarMap:
    def __init__(self, root):
        self.root = root
        self.root.title("QNX ADS-B Ground Station")
        self.canvas = tk.Canvas(root, width=600, height=600, bg="black")
        self.canvas.pack()
        
        # Setup Socket
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.bind(('', PORT))
            self.sock.setblocking(False)
            print(f"Listening for QNX data on port {PORT}...")
        except Exception as e:
            print(f"Socket Setup Error: {e}")
        
        # Center of map (Matches your Transmitter start point)
        self.ref_lat = 34.0522
        self.ref_lon = -118.2437
        self.drones = {} 
        self.update_radar()

    def update_radar(self):
        try:
            data, addr = self.sock.recvfrom(1024)
            
            # DEBUG: This will show us the exact byte count in your terminal
            # print(f"DEBUG: Received {len(data)} bytes from {addr}")
            
            # Unpack the binary data
            unpacked = struct.unpack(ADSB_STRUCT_FMT, data)
            d_id = unpacked[0]
            lat = unpacked[1]
            lon = unpacked[2]
            
            # Convert Lat/Lon to Pixel Coordinates (Zoom level 10000)
            x = (lon - self.ref_lon) * 10000 + 300
            y = 300 - (lat - self.ref_lat) * 10000
            
            if d_id not in self.drones:
                self.drones[d_id] = self.canvas.create_oval(x-5, y-5, x+5, y+5, fill="green")
                self.canvas.create_text(x, y-10, text=f"Drone {d_id}", fill="white", tags=f"label{d_id}")
                print(f"Drawing New Drone ID: {d_id}")
            else:
                self.canvas.coords(self.drones[d_id], x-5, y-5, x+5, y+5)
                self.canvas.coords(f"label{d_id}", x, y-10)
                
        except (BlockingIOError, socket.error):
            pass 
        except Exception as e:
            print(f"Unpack Error: {e}")
        
        self.root.after(100, self.update_radar)

if __name__ == "__main__":
    root = tk.Tk()
    app = RadarMap(root)
    root.mainloop()
