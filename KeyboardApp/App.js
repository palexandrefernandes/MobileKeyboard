import TcpSocket from 'react-native-tcp-socket';

import React, { useEffect } from 'react';
import {
  StyleSheet,
  View,
  Text,
  TextInput,
  Button,
  BackHandler
} from 'react-native';

const serverOptions = {
	port: 27016,
	host: ""
}

let client;

console.disableYellowBox = true;

const App = () => {
	const [ip, setIp] = React.useState('192.168.1.');
	const [isConnected, setConnected] = React.useState(false);
	const [text, setText] = React.useState("");

	const connectServer = () => {
		serverOptions.host = ip;
		client = TcpSocket.createConnection(serverOptions);

		client.on('data', (data) => {
			if(data){
				console.log(Buffer.from(data).toString());
			}
		});

		client.on('close', () => {
			setConnected(false);
		});

		setConnected(true);
	};

	const sendToServer = () => {
		client.write(text);
		setText("");
	};
	
	const deleteKey = () => {
		console.log("TODO");
		//client.write()
	};

	useEffect(() => {
		BackHandler.addEventListener('hardwareBackPress', () => {
			if(client){
				client.destroy();
				client = null;
				return true;
			}
			return false;
		});
	}, []);

	return (
		<View style={styles.container}>
			{!isConnected ? (
				<View style={styles.connectContainer}>
					<Text style={styles.labels}>Ip address:</Text>
					<TextInput style={styles.input} onChangeText={(text) => setIp(text)} value={ip} maxLength={15}/>
					<Button color={"#EF8354"} title="Connect" onPress={connectServer}/>
				</View>
			) :(
				<View style={styles.sendInputContainer}>
					<Text style={styles.labels}>Input</Text>
					<TextInput blurOnSubmit={false} style={[styles.input, {textAlignVertical: 'top'}]} onChangeText={(text) => {setText(text)}} value={text} onSubmitEditing={sendToServer} scrollEnabled numberOfLines={10} maxLength={512}/>
				</View>
			)}



		</View>
	);
};

//<Button color={"#EF8354"} title="Delete" onPress={deleteKey}/>

const styles = StyleSheet.create({
	container: {
		backgroundColor: "#2D3142",
		flex: 1,
		paddingHorizontal: 20,
		color: "#2D3142",
		justifyContent: 'center',
	},
	connectContainer: {
		color: "#FFFFFF"
	},
	labels: {
		color: "#EF8354",
		fontSize: 18,
	},
	input: {
		backgroundColor: "#565A68",
		color: "#FFF",
		marginBottom:10,
		borderColor: 'transparent',
		borderBottomColor: "#EF8354",
		borderWidth: 2,
		borderRadius: 5,
		paddingHorizontal: 10
	},
	sendInputContainer:{

	}
})

export default App;
