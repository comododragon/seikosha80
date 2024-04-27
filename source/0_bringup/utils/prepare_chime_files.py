#!/usr/bin/env python3


import os, subprocess, sys, tempfile, wave


if "__main__" == __name__:
	if len(sys.argv) != 4:
		sys.stderr.write("Usage: {} CHIMESFOLDER SDCARDIMG INDEXH\n")
		sys.stderr.write("\n")
		sys.stderr.write("    where CHIMESFOLDER points to a folder containing\n")
		sys.stderr.write("    15 wav files, named as follows:\n")
		sys.stderr.write("        quarter.wav\n")
		sys.stderr.write("        half.wav\n")
		sys.stderr.write("        threequarters.wav\n")
		sys.stderr.write("        01hour.wav\n")
		sys.stderr.write("        02hour.wav\n")
		sys.stderr.write("        03hour.wav\n")
		sys.stderr.write("        04hour.wav\n")
		sys.stderr.write("        05hour.wav\n")
		sys.stderr.write("        06hour.wav\n")
		sys.stderr.write("        07hour.wav\n")
		sys.stderr.write("        08hour.wav\n")
		sys.stderr.write("        09hour.wav\n")
		sys.stderr.write("        10hour.wav\n")
		sys.stderr.write("        11hour.wav\n")
		sys.stderr.write("        12hour.wav\n")
		sys.stderr.write("\n")
		sys.stderr.write("The names are self-explanatory.\n")
		sys.stderr.write("These files will be concatenated in this order to a binary image\n")
		sys.stderr.write("1 channel, 8 bit, 8000kHz\n")
		sys.stderr.write("Each wave will have 5 appended seconds of silence\n")
		sys.stderr.write("Before each wave file, an unsigned long is placed with the size in bytes of the wave file\n")
		sys.stderr.write("This binary image can be dd'ed to an sdcard\n")
		sys.stderr.write("And an \"index.h\" file will be generated with the file table\n")
		exit(-1)

	waveFolder = sys.argv[1]
	sdcardImg = sys.argv[2]
	indexH = sys.argv[3]
	indexes = []
	index = 0
	binaryFile = bytes()

	for fileName in ["quarter.wav", "half.wav", "threequarters.wav", *["{:02}hour.wav".format(x) for x in range(1, 13)]]:
		filePath = os.path.join(waveFolder, fileName)

		if not os.path.exists(filePath):
			sys.stderr.write("Error: file not found: {}".format(filePath))

		print("Now processing {}".format(fileName))

		with tempfile.NamedTemporaryFile(mode="rb", suffix=".wav") as tmpF:
			print("    conversion output will be saved at {}".format(tmpF.name))
			subprocess.run(["ffmpeg", "-i", filePath, "-acodec", "pcm_u8", "-ac", "1", "-ar", "8000", "-y", tmpF.name])
			print("    done!")

			with wave.open(tmpF.name, "rb") as wavF:
				frames = wavF.readframes(-1)

				print("    wave file size: {} bytes".format(len(frames)))
				totalWavFileSize = len(frames) + 5 * 8000
				print("    wave file size (with 5s silence): {} bytes".format(totalWavFileSize))
				totalWavFileSize += (512 - (totalWavFileSize % 512))
				print("    wave file size (with 5s silence, 512-byte aligned): {} bytes".format(totalWavFileSize))
				print("    wave file address: {}".format(index))
				indexes.append(index)
				index += totalWavFileSize

				print("    writing wave file")
				binaryFile += frames
				print("    writing 5s silence, 512-byte aligned")
				binaryFile += bytes([128] * (totalWavFileSize - len(frames)))

	print("Total binary file size: {} bytes".format(len(binaryFile)))

	print("Writing to output {}".format(sdcardImg))
	with open(sdcardImg, "wb") as imgF:
		imgF.write(binaryFile)

	print("Writing to {}".format(indexH))
	with open(indexH, "w") as hF:
		hF.write("/**\n")
		hF.write(" * Index for sdcard contents\n")
		hF.write(" * auto-prepared using prepare.py\n")
		hF.write(" */\n")
		hF.write("\n")
		hF.write("\n")
		hF.write("#ifndef INDEX_H\n")
		hF.write("#define INDEX_H\n")
		hF.write("\n")
		hF.write("\n")
		hF.write("// Last index points to EOF\n")
		hF.write("unsigned long sdcardIndex[16] = {\n")
		for i in indexes:
			hF.write("\t0x{:08X},\n".format(i))
		hF.write("\t0x{:08X}\n".format(index))
		hF.write("};\n")
		hF.write("\n")
		hF.write("\n")
		hF.write("#endif\n")
