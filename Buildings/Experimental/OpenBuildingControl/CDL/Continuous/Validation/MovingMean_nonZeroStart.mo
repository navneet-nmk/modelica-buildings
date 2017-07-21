within Buildings.Experimental.OpenBuildingControl.CDL.Continuous.Validation;
model MovingMean_nonZeroStart
  "Validation model for the MovingMean block"
extends Modelica.Icons.Example;

  Modelica.Blocks.Sources.Sine sinInpNoDel(freqHz=1/80)
    "Start from zero second"
    annotation (Placement(transformation(extent={{-90,50},{-70,70}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_1(delta=100)
    "Moving average with 100 s sliding window"
    annotation (Placement(transformation(extent={{-40,50},{-20,70}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_2(delta=200)
    "Moving average with 200 s sliding window"
    annotation (Placement(transformation(extent={{-40,20},{-20,40}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_3(delta=300)
    "Moving average with 300 s sliding window"
    annotation (Placement(transformation(extent={{-40,-10},{-20,10}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_4(delta=400)
    "Moving average with 400 s sliding window"
    annotation (Placement(transformation(extent={{-40,-40},{-20,-20}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_5(delta=500)
    "Moving average with 500 s sliding window"
    annotation (Placement(transformation(extent={{-40,-70},{-20,-50}})));
  Modelica.Blocks.Sources.Sine sinInp50sDel(freqHz=1/80, startTime=50)
    "Start from 50 seconds"
    annotation (Placement(transformation(extent={{0,50},{20,70}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_6(
    delta=100) "Moving average with 100 s sliding window"
    annotation (Placement(transformation(extent={{60,50},{80,70}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_7(
    delta=200) "Moving average with 200 s sliding window"
    annotation (Placement(transformation(extent={{60,20},{80,40}})));
  Modelica.Blocks.Sources.Sine sinInp100sDel(freqHz=1/80, startTime=100)
    "Start from 100 seconds"
    annotation (Placement(transformation(extent={{0,-30},{20,-10}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_8(
    delta=100) "Moving average with 100 s sliding window"
    annotation (Placement(transformation(extent={{60,-30},{80,-10}})));
  Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean movingMean_9(
    delta=200) "Moving average with 200 s sliding window"
    annotation (Placement(transformation(extent={{60,-60},{80,-40}})));

equation
  connect(sinInpNoDel.y, movingMean_3.u) annotation (Line(points={{-69,60},{-60,
          60},{-60,0},{-42,0}}, color={0,0,127}));
  connect(sinInpNoDel.y, movingMean_1.u)
    annotation (Line(points={{-69,60},{-42,60}},          color={0,0,127}));
  connect(sinInpNoDel.y, movingMean_2.u) annotation (Line(points={{-69,60},{-60,
          60},{-60,30},{-42,30}},          color={0,0,127}));
  connect(sinInpNoDel.y, movingMean_5.u) annotation (Line(points={{-69,60},{-60,
          60},{-60,-60},{-42,-60}}, color={0,0,127}));
  connect(sinInpNoDel.y, movingMean_4.u) annotation (Line(points={{-69,60},{-60,
          60},{-60,-30},{-42,-30}}, color={0,0,127}));
  connect(sinInp50sDel.y, movingMean_6.u)
    annotation (Line(points={{21,60},{21,60},{58,60}}, color={0,0,127}));
  connect(sinInp50sDel.y, movingMean_7.u) annotation (Line(points={{21,60},{21,60},
          {40,60},{40,30},{58,30}}, color={0,0,127}));
  connect(sinInp100sDel.y, movingMean_9.u) annotation (Line(points={{21,-20},{21,
          -20},{40,-20},{40,-50},{58,-50}}, color={0,0,127}));
  connect(sinInp100sDel.y, movingMean_8.u)
    annotation (Line(points={{21,-20},{58,-20}}, color={0,0,127}));
  annotation (experiment(StartTime=100, StopTime=900.0, Tolerance=1e-06),
  __Dymola_Commands(file="modelica://Buildings/Resources/Scripts/Dymola/Experimental/OpenBuildingControl/CDL/Continuous/Validation/MovingMean_nonZeroStart.mos"
        "Simulate and plot"),
    Documentation(info="<html>
<p>
Validation test for the block
<a href=\"modelica://Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean\">
Buildings.Experimental.OpenBuildingControl.CDL.Continuous.MovingMean</a>.
</p>
<p>
The inputs <code>sinInpNoDel</code>, <code>sinInp50sDel</code> and
<code>sinInp100sDel</code> generate sine outputs with the same frequency of
<code>1/80</code> Hertz, but different start times of <code>0</code> seconds,
<code>50</code> seconds and <code>100</code> seconds.
</p>
</html>", revisions="<html>
<ul>
<li>
June 29, 2017, by Jianjun Hu:<br/>
First implementation.
</li>
</ul>
</html>"));
end MovingMean_nonZeroStart;