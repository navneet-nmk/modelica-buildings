within Buildings.ChillerWSE.Examples.BaseClasses.Controls;
model CoolingTowerSpeedControl "Controller for the fan speed in cooling towers"
  extends Modelica.Blocks.Icons.Block;

  parameter Modelica.Blocks.Types.SimpleController controllerType=
    Modelica.Blocks.Types.SimpleController.PID
    "Type of controller"
    annotation(Dialog(tab="Controller"));
  parameter Real k(min=0, unit="1") = 1
    "Gain of controller"
    annotation(Dialog(tab="Controller"));
  parameter Modelica.SIunits.Time Ti(min=Modelica.Constants.small)=0.5
    "Time constant of Integrator block"
     annotation (Dialog(enable=
          (controllerType == Modelica.Blocks.Types.SimpleController.PI or
          controllerType == Modelica.Blocks.Types.SimpleController.PID),tab="Controller"));
  parameter Modelica.SIunits.Time Td(min=0)=0.1
    "Time constant of Derivative block"
     annotation (Dialog(enable=
          (controllerType == Modelica.Blocks.Types.SimpleController.PD or
          controllerType == Modelica.Blocks.Types.SimpleController.PID),tab="Controller"));
  parameter Real yMax(start=1)=1
   "Upper limit of output"
    annotation(Dialog(tab="Controller"));
  parameter Real yMin=0
   "Lower limit of output"
    annotation(Dialog(tab="Controller"));
  parameter Boolean reverseAction = true
    "Set to true for throttling the water flow rate through a cooling coil controller"
    annotation(Dialog(tab="Controller"));
  Modelica.Blocks.Interfaces.RealInput TCHWSupSet(
    final quantity="ThermodynamicTemperature",
    final unit="K",
    displayUnit="degC") "Chilled water supply temperature setpoint"
    annotation (Placement(transformation(extent={{-140,-20},{-100,20}})));
  Modelica.Blocks.Interfaces.RealInput TCWSupSet(
    final quantity="ThermodynamicTemperature",
    final unit="K",
    displayUnit="degC") "Condenser water supply temperature setpoint"
    annotation (Placement(transformation(extent={{-140,60},{-100,100}})));
  Modelica.Blocks.Interfaces.RealInput TCHWSup(
    final quantity="ThermodynamicTemperature",
    final unit="K",
    displayUnit="degC") "Chilled water supply temperature " annotation (
      Placement(transformation(
        extent={{-20,-20},{20,20}},
        rotation=0,
        origin={-120,-74}), iconTransformation(extent={{-140,-100},{-100,-60}})));
  Modelica.Blocks.Interfaces.RealInput TCWSup(
    final quantity="ThermodynamicTemperature",
    final unit="K",
    displayUnit="degC") "Condenser water supply temperature " annotation (
      Placement(transformation(
        extent={{20,20},{-20,-20}},
        rotation=180,
        origin={-120,-40})));
  Modelica.Blocks.Interfaces.RealOutput y
    "Speed signal for cooling tower fans"
    annotation (Placement(transformation(extent={{100,-10},{120,10}})));

  Modelica.Blocks.Sources.Constant uni(k=1) "Unit"
    annotation (Placement(transformation(extent={{20,30},{40,50}})));
  Modelica.Blocks.Sources.BooleanExpression pmcMod(
    y=if cooMod == 2 then true else false)
    "Partially mechanical cooling mode"
    annotation (Placement(transformation(extent={{20,-10},{40,10}})));

  Modelica.Blocks.Interfaces.IntegerInput cooMod
    "Cooling mode signal, integer value of 
    Buildings.Applications.DataCenters.Examples.BaseClasses.Types.CoolingMode"
    annotation (Placement(transformation(extent={{-140,20},{-100,60}})));
  Buildings.Controls.Continuous.LimPID conPID(
    controllerType=controllerType,
    k=k,
    Ti=Ti,
    Td=Td,
    yMax=yMax,
    yMin=yMin,
    reverseAction=reverseAction)
    "PID controller"
    annotation (Placement(transformation(extent={{20,-50},{40,-30}})));
  Modelica.Blocks.Math.IntegerToBoolean fmcMod(threshold=3)
    "Fully mechanical cooling mode"
    annotation (Placement(transformation(extent={{-80,30},{-60,50}})));

protected
  Modelica.Blocks.Logical.Switch swi1
    "Switch 1"
    annotation (Placement(transformation(extent={{-30,40},{-10,60}})));
  Modelica.Blocks.Logical.Switch swi2
    "Switch 2"
    annotation (Placement(transformation(extent={{-10,-10},{10,10}},
        rotation=0,
        origin={-30,-60})));
  Modelica.Blocks.Logical.Switch swi3
    "Switch 3"
    annotation (Placement(transformation(extent={{-10,-10},{10,10}},
        rotation=0,
        origin={74,0})));

equation
  connect(TCWSupSet, swi1.u1)
    annotation (Line(points={{-120,80},{-48,80},{-48,
          58},{-32,58}}, color={0,0,127}));
  connect(TCHWSupSet, swi1.u3)
    annotation (Line(points={{-120,0},{-48,0},{-48,
          42},{-32,42}}, color={0,0,127}));
  connect(swi1.y, conPID.u_s)
    annotation (Line(points={{-9,50},{0,50},{0,-40},{18,
          -40}}, color={0,0,127}));
  connect(fmcMod.y, swi2.u2)
    annotation (Line(points={{-59,40},{-54,40},{-54,-60},
          {-42,-60}}, color={255,0,255}));
  connect(TCWSup, swi2.u1)
    annotation (Line(points={{-120,-40},{-50,-40},{-50,-52},
          {-42,-52}}, color={0,0,127}));
  connect(TCHWSup, swi2.u3)
    annotation (Line(points={{-120,-74},{-120,-68},{-50,
          -68},{-42,-68}}, color={0,0,127}));
  connect(swi2.y, conPID.u_m)
    annotation (Line(points={{-19,-60},{30,-60},{30,-52}}, color={0,0,127}));
  connect(pmcMod.y, swi3.u2)
    annotation (Line(points={{41,0},{51.5,0},{62,0}}, color={255,0,255}));
  connect(uni.y, swi3.u1)
    annotation (Line(points={{41,40},{50,40},{50,8},{62,8}}, color={0,0,127}));
  connect(conPID.y, swi3.u3)
    annotation (Line(points={{41,-40},{50,-40},{50,-8},
          {62,-8}}, color={0,0,127}));
  connect(swi3.y, y)
    annotation (Line(points={{85,0},{110,0}}, color={0,0,127}));
  connect(fmcMod.y, swi1.u2)
    annotation (Line(points={{-59,40},{-54,40},{-54,50},
          {-32,50}}, color={255,0,255}));
  connect(cooMod, fmcMod.u)
    annotation (Line(points={{-120,40},{-102,40},{-82,
          40}}, color={255,127,0}));
  annotation (Icon(coordinateSystem(preserveAspectRatio=false, extent={{-100,
            -100},{100,80}})),                                   Diagram(
        coordinateSystem(preserveAspectRatio=false, extent={{-100,-100},{100,
            100}})),
    Documentation(info="<html>
<p>This model describes a simple cooling tower speed controller for 
a chilled water system with integrated waterside economizers.
</p>
<p>The control logics are described in the following:</p>
<ul>
<li>When the system is in Fully Mechanical Cooling (FMC) mode, 
the cooling tower fan speed is controlled to maintain the condener water supply temperature (CWST) 
at or around the setpoint.
</li>
<li>When the system is in Partially Mechanical Cooling (PMC) mode, 
the cooling tower fan speed is set as 100%; to make condenser water 
as cold as possible and maximize the waterside economzier output.
</li>
<li>When the system is in Free Cooling (FC) mode, 
the cooling tower fan speed is controlled to maintain the chilled water supply temperature (CHWST) 
at or around its setpoint.
</li>
</ul>
</html>", revisions="<html>
<ul>
<li>
July 30, 2017, by Yangyang Fu:<br/>
First implementation.
</li>
</ul>
</html>"));
end CoolingTowerSpeedControl;
