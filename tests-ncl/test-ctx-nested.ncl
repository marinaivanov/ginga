<ncl>
  <head>
    <connectorBase>
      <causalConnector xconnector="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="ctx0"/>
    <context id="ctx0">
      <port id="start0a" component="time0"/>
      <port id="start0b" component="ctx1"/>
      <media id="time0">
        <area id="a0" begin="4s"/>
        <property name="bounds" value="10%,10%,100%,100%"/>
        <property name="background" value="red"/>
      </media>
      <context id="ctx1">
        <port id="start1a" component="time1"/>
        <port id="start1b" component="ctx2"/>
        <media id="time1">
          <area id="a1" begin="3s"/>
          <property name="bounds" value="20%,20%,100%,100%"/>
          <property name="background" value="green"/>
        </media>
        <context id="ctx2">
          <port id="start2a" component="time2"/>
          <port id="start2b" component="ctx3"/>
          <media id="time2">
            <area id="a2" begin="2s"/>
            <property name="bounds" value="30%,30%,100%,100%"/>
            <property name="background" value="blue"/>
          </media>
          <context id="ctx3">
            <port id="start3a" component="time3"/>
            <media id="time3">
              <area id="a3" begin="1s"/>
              <property name="bounds" value="40%,40%,100%,100%"/>
              <property name="background" value="yellow"/>
            </media>
            <link>
              <bind role="onBegin" component="time3" interface="a3"/>
              <bind role="stop" component="ctx3"/>
            </link>
          </context>
          <link>
            <bind role="onBegin" component="time2" interface="a2"/>
            <bind role="stop" component="ctx2"/>
          </link>
        </context>
        <link>
          <bind role="onBegin" component="time1" interface="a1"/>
          <bind role="stop" component="ctx1"/>
        </link>
      </context>
      <link>
        <bind role="onBegin" component="time0" interface="a0"/>
        <bind role="stop" component="ctx0"/>
      </link>
    </context>
  </body>
</ncl>