<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Generated by NCL Eclipse -->
<ncl id="test-player-siggen-bit">
<head>
</head>
<body>
  <port id="start" component="m"/>
  <port id ="start2" component = "m2" />
  <port id ="start3" component = "m3" />
  <port id ="start4" component = "m4" />
  <media id="m" type="application/x-ginga-siggen">
    <property name="freq" value="20000"/>
    <property name="volume" value="0.8"/>
    
  </media>
  <media id="m2" type="application/x-ginga-siggen">
    <property name="freq" value="20200"/>
    <property name="volume" value="0.8"/>

  </media>

  <media id="m3" type="application/x-ginga-siggen">
    <property name="freq" value="20400"/>
    <property name="volume" value="0.8"/>

  </media>

  <media id="m4" type="application/x-ginga-siggen">
    <property name="freq" value="20600"/>
    <property name="volume" value="0.8"/>

  </media>
</body>

</ncl>
