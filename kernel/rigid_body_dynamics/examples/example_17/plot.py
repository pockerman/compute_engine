import csv
import matplotlib.pyplot as plt

dt = 0.5

def main():
	

	with open('vehicle_state.csv', 'r') as file:
		csv_reader = csv.reader(file, delimiter=",")
         
		time = 0.0
		counter = 0
		for line in csv_reader:

			plt.cla()
			plt.gcf().canvas.mpl_connect('key_release_event',
                    lambda event: [exit(0) if event.key == 'escape' else None])
                    
			x = float(line[0])
			y = float(line[1])
			
			print(x, ",", y)
			
			x1.append(x)
			y1.append(y)
			plt.plot(x1, y1, ".-g")
			plt.legend(["time %s " %time])
			plt.axis("equal")
			plt.grid(True)
			plt.pause(0.001)
			time += dt
			
	import imageio
	images = []
	
	# you must generate the files below
	filenames = [   "Figure_1.png",
					"Figure_2.png",
					"Figure_3.png",
					"Figure_4.png",
					"Figure_5.png",
					"Figure_6.png",
					"Figure_7.png",
					"Figure_8.png",
					"Figure_9.png",  
					 "Figure_100.png",
					"Figure_101.png",
					"Figure_102.png",
					"Figure_103.png",
					"Figure_104.png",
					"Figure_105.png",
					"Figure_106.png",
					"Figure_107.png",
					"Figure_108.png",
					"Figure_109.png",
					]
	
	for filename in filenames:
		images.append(imageio.imread(filename))
	imageio.mimsave('movie.gif', images)
	
	
if __name__ == '__main__':
	main()	
