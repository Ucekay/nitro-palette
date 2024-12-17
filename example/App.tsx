import { useState } from "react";
import {
	Button,
	FlatList,
	Image,
	SafeAreaView,
	StyleSheet,
	Text,
	View,
	useWindowDimensions,
} from "react-native";
import RNColorThief from "react-native-color-thief";
import { getPaletteAsync } from "react-native-nitro-palette";
import { benchmark } from "./utils/performance-benchmark";

const IMAGES = [
	"https://images.unsplash.com/photo-1551410224-699683e15636?q=80&w=2264&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D",
	"https://images.unsplash.com/photo-1721641843496-3c8c60eab024?q=80&w=2274&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D",
	"https://images.unsplash.com/photo-1542051841857-5f90071e7989?q=80&w=2370&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D",
	"https://images.unsplash.com/photo-1731329396266-8250ea4d56c1?q=80&w=2487&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D",
	"https://images.unsplash.com/photo-1500462918059-b1a0cb512f1d?q=80&w=2487&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D",
	"https://images.unsplash.com/photo-1525909002-1b05e0c869d8?q=80&w=2448&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D",
];

export default function App() {
	const [palette, setPalette] = useState<string[] | null>(null);
	const [index, setIndex] = useState(0);
	const { width } = useWindowDimensions();
	const [nitroTime, setNitroTime] = useState<number | null>(null);
	const [thiefTime, setThiefTime] = useState<number | null>(null);

	const handleGet = async () => {
		const result = await benchmark.measure(
			"nitro-palette",
			async () => {
				try {
					const colors = await getPaletteAsync(IMAGES[index], 5, 1, true);
					setPalette(colors);
				} catch (error) {
					console.error(error);
				}
			},
			10,
		);
		console.log("nitro palette");
		console.log(`平均実行時間: ${result.averageTime.toFixed(2)}ms`);
		console.log(`最小実行時間: ${result.minTime.toFixed(2)}ms`);
		console.log(`最大実行時間: ${result.maxTime.toFixed(2)}ms`);
	};

	const handleThief = async () => {
		const result = await benchmark.measure(
			"color-thief",
			async () => {
				try {
					const colors = await RNColorThief.getPalette(
						IMAGES[index],
						5,
						1,
						true,
					);
					const rgbColors = colors.map(
						(color) => `rgb(${color.r}, ${color.g}, ${color.b})`,
					);
					setPalette(rgbColors);
				} catch (error) {
					console.error(error);
				}
			},
			10,
		);
		console.log("color thief");
		console.log(`平均実行時間: ${result.averageTime.toFixed(2)}ms`);
		console.log(`最小実行時間: ${result.minTime.toFixed(2)}ms`);
		console.log(`最大実行時間: ${result.maxTime.toFixed(2)}ms`);
	};

	return (
		<SafeAreaView style={{ flex: 1, backgroundColor: "white" }}>
			<View style={styles.container}>
				<FlatList
					pagingEnabled
					data={IMAGES}
					renderItem={({ item }) => (
						<View style={{ width, aspectRatio: 1, padding: 16 }}>
							<View
								style={{
									borderRadius: 20,
									borderCurve: "continuous",
									boxShadow: "0 4 10 4 #00000050",
								}}
							>
								<View
									style={{
										borderRadius: 20,
										borderCurve: "continuous",
										overflow: "hidden",
									}}
								>
									<Image source={{ uri: item }} style={styles.image} />
								</View>
							</View>
						</View>
					)}
					keyExtractor={(item) => item}
					horizontal
					showsHorizontalScrollIndicator={false}
					onMomentumScrollEnd={(event) => {
						const index = Math.floor(
							Math.floor(event.nativeEvent.contentOffset.x) /
								Math.floor(event.nativeEvent.layoutMeasurement.width),
						);
						setIndex(index);
					}}
				/>
				<View style={{ flex: 1, paddingHorizontal: 20, marginVertical: 16 }}>
					<View style={{ flex: 1, justifyContent: "center" }}>
						<View style={styles.paletteContainer}>
							<Text>
								Time to get palette:{" "}
								{nitroTime ? nitroTime : thiefTime ? thiefTime : ""}{" "}
							</Text>
						</View>
					</View>
					{palette && (
						<View style={styles.paletteContainer}>
							{palette.map((color, index) => (
								<View
									key={index}
									style={[{ backgroundColor: color }, styles.block]}
								/>
							))}
						</View>
					)}
				</View>
				<Button title="Get Palette with nitro-palette" onPress={handleGet} />
				<Button title="Get Palette with ColorThief" onPress={handleThief} />
				<Button
					title="Clear Palette"
					onPress={() => {
						setPalette(null);
					}}
				/>
			</View>
		</SafeAreaView>
	);
}

const styles = StyleSheet.create({
	container: {
		flex: 1,
		backgroundColor: "#fff",
		alignItems: "center",
		justifyContent: "flex-start",
	},
	image: {
		width: "100%",
		aspectRatio: 1,
	},
	paletteContainer: {
		flexDirection: "row",
		flexWrap: "wrap",
		justifyContent: "center",
		gap: 12,
	},
	block: {
		width: 50,
		height: 50,
		borderRadius: 10,
		borderCurve: "continuous",
	},
	performance: {
		paddingVertical: 20,
		gap: 12,
	},
});
