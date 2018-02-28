<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="prefetchExample" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="regVideo" width="100%" height="100%" zIndex="1"/>
			<region id="regLoading" top="45%" left="45%" width="10%" height="10%" zIndex="2"/>
			<region id="regBtnPlay" top="45%" left="35%" width="30%" height="13%" zIndex="2"/>
		</regionBase>

		<descriptorBase>
			<descriptor id="descVideo" region="regVideo"/>
			<descriptor id="descLoading" region="regLoading" explicitDur="10s"/>
			<descriptor id="descBtnPlay" region="regBtnPlay"/>
		</descriptorBase>

		<connectorBase>
			<causalConnector id="onBeginStartPrefetch">
				<simpleCondition role="onBegin"/>
				<simpleAction role="startPrefetch"/>
			</causalConnector>
			<causalConnector id="onBeginPrefetchStart">
				<simpleCondition role="onBeginPrefetch"/>
				<simpleAction role="start"/>
			</causalConnector>
			<causalConnector id="onEndPrefetchStart">
				<simpleCondition role="onEndPrefetch"/>
				<simpleAction role="start"/>
			</causalConnector>
			<causalConnector id="onSelectionStartPrefetchStop">
				<simpleCondition role="onSelection"/>
				<simpleAction role="startPrefetch"/>
				<simpleAction role="stop"/>
			</causalConnector>
		</connectorBase>
	</head>
	<body>
		<port id="port1" component="btnPlay"/>

		<media id="video" src="samples/clock.ogv" descriptor="descVideo">
			<area id="area1" end="5s"/>
		</media>
		<media id="btnPlay" src="samples/gnu.png" descriptor="descBtnPlay"/>
		<media id="loading" src="samples/bunny.ogg" descriptor="descLoading"/>

		<link xconnector="onBeginStartPrefetch">
			<bind role="onBegin" component="btnPlay"/>
			<bind role="startPrefetch" component="loading"/>
		</link>

		<link xconnector="onSelectionStartPrefetchStop">
			<bind role="onSelection" component="btnPlay"/>
			<bind role="startPrefetch" component="video" interface="area1"/>
			<bind role="stop" component="btnPlay"/>
		</link>

		<link xconnector="onBeginPrefetchStart">
			<bind role="onBeginPrefetch" component="video" interface="area1"/>
			<bind role="start" component="loading"/>
		</link>

		<link xconnector="onEndPrefetchStart">
			<bind role="onEndPrefetch" component="video" interface="area1"/>
			<bind role="stop" component="loading"/>
			<bind role="start" component="video"/>
		</link>
	</body>
</ncl>
