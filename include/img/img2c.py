from PIL import Image
import sys
import os

def main():
    if len(sys.argv) < 2:
        print("Kullanim: python img2c.py <resim_dosyasi>")
        return

    img_path = sys.argv[1]
    name = os.path.splitext(os.path.basename(img_path))[0]
    
    # Resmi aç ve RGBA (32-bit) formatına çevir
    try:
        img = Image.open(img_path).convert("RGBA")
    except Exception as e:
        print(f"Hata: {e}")
        return

    width, height = img.size
    print(f"İşleniyor: {name} ({width}x{height})")

    # C Header dosyasını oluştur
    with open(f"{name}.h", "w") as f:
        f.write(f"// {name} ({width}x{height})\n")
        f.write("#include <stdint.h>\n\n")
        
        # BMP Header'ı manuel oluşturuyoruz (Daha güvenli)
        file_size = 54 + (width * height * 4)
        
        header = bytearray([
            0x42, 0x4D,             # BM
            file_size & 0xFF, (file_size >> 8) & 0xFF, (file_size >> 16) & 0xFF, (file_size >> 24) & 0xFF,
            0x00, 0x00, 0x00, 0x00, # Reserved
            0x36, 0x00, 0x00, 0x00, # Offset (54)
            0x28, 0x00, 0x00, 0x00, # Info Header Size (40)
            width & 0xFF, (width >> 8) & 0xFF, (width >> 16) & 0xFF, (width >> 24) & 0xFF,
            # Height (Negatif = Top-Down, böylece resim ters dönmez)
            (0 - height) & 0xFF, ((0 - height) >> 8) & 0xFF, ((0 - height) >> 16) & 0xFF, ((0 - height) >> 24) & 0xFF,
            0x01, 0x00,             # Planes
            0x20, 0x00,             # 32-bit (RGBA)
            0x00, 0x00, 0x00, 0x00, # Compression
            0x00, 0x00, 0x00, 0x00, # Image Size (Dummy)
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        ])

        f.write(f"const uint8_t {name}_bmp[] = {{\n")
        
        # Header'ı yaz
        for i, b in enumerate(header):
            f.write(f"0x{b:02X}, ")
            if (i + 1) % 12 == 0: f.write("\n")
        f.write("\n\n")

        # Pikselleri yaz (BGRA sırasıyla çünkü BMP Little Endian)
        data = list(img.getdata())
        count = 0
        for r, g, b, a in data:
            # BMP bellekte B-G-R-A olarak tutulur
            f.write(f"0x{b:02X},0x{g:02X},0x{r:02X},0x{a:02X}, ")
            count += 1
            if count % 4 == 0: f.write("\n")

        f.write("};\n")
        print(f"Oluşturuldu: {name}.h")

if __name__ == "__main__":
    main()