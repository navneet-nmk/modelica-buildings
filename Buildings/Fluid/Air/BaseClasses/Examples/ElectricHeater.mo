within Buildings.Fluid.Air.BaseClasses.Examples;
model ElectricHeater "Test model for ElectricHeater"
  extends Modelica.Icons.Example;
  extends Buildings.Fluid.HeatExchangers.Examples.BaseClasses.Heater(
    redeclare package Medium = Buildings.Media.Air,
    m_flow_nominal=V*1.2*6/3600,
    Q_flow_nominal=30*6*6,
    mov(nominalValuesDefineDefaultPressureCurve=true,
    dp_nominal=1200));
  Buildings.Fluid.Air.BaseClasses.ElectricHeater eleHea(
    redeclare package Medium = Medium,
    m_flow_nominal=m_flow_nominal,
    QMax_flow=Q_flow_nominal,
    eta=1.0,
    energyDynamics=Modelica.Fluid.Types.Dynamics.FixedInitial,
    dp_nominal=1000)
    "Electric heater"
    annotation (Placement(transformation(extent={{-20,-50},{0,-30}})));
   Buildings.Controls.SetPoints.Table tab(table=[0,273.15 + 15; 1,273.15 + 30])
    "Temperature set point"
    annotation (Placement(transformation(extent={{-30,20},{-10,40}})));
  Modelica.Blocks.Sources.BooleanStep onOff(startTime(displayUnit="min")=
      60000) "On/off signal"
    annotation (Placement(transformation(extent={{-80,-98},{-60,-78}})));
equation
  connect(mov.port_b, eleHea.port_a) annotation (Line(points={{-50,-40},{-35,-40},
          {-20,-40}}, color={0,127,255}));
  connect(eleHea.port_b, THeaOut.port_a)
    annotation (Line(points={{0,-40},{20,-40}}, color={0,127,255}));
  connect(conPI.y, tab.u)
    annotation (Line(points={{-39,30},{-35.5,30},{-32,30}}, color={0,0,127}));
  connect(tab.y, eleHea.TSet) annotation (Line(points={{-9,30},{-4,30},{-4,-16},
          {-30,-16},{-30,-32},{-22,-32}}, color={0,0,127}));
  connect(onOff.y, eleHea.On) annotation (Line(points={{-59,-88},{-36,-88},{-36,
          -37},{-22,-37}}, color={255,0,255}));
  annotation (Icon(coordinateSystem(preserveAspectRatio=false)), Diagram(
        coordinateSystem(preserveAspectRatio=false)),
    __Dymola_Commands(file=
          "modelica://Buildings/Resources/Scripts/Dymola/Fluid/Air/BaseClasses/Examples/ElectricHeater.mos"
        "Simulate and Plot"),
    Documentation(info="<html>
This model test the electric heater model: <a href=\"modelica://Buildings.Fluid.Air.BaseClasses.ElectricHeater\">
Buildings.Fluid.Air.BaseClasses.ElectricHeater</a>.
</html>", revisions="<html>
<ul>
<li>
April 11, 2017 by Yangyang Fu:<br/>
First implementation.
</li>
</ul>
</html>"));
end ElectricHeater;
