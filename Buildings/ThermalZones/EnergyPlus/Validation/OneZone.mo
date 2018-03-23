within Buildings.ThermalZones.EnergyPlus.Validation;
model OneZone "Validation model for one zone"
  extends Modelica.Icons.Example;
  package Medium = Buildings.Media.Air "Medium model";

  parameter String fmuName = "aaa.fmu" "Name of the FMU file that contains this zone";

  Modelica.Blocks.Sources.Constant qConGai_flow(k=0) "Convective heat gain"
    annotation (Placement(transformation(extent={{-80,0},{-60,20}})));
  Modelica.Blocks.Sources.Constant qRadGai_flow(k=0) "Radiative heat gain"
    annotation (Placement(transformation(extent={{-80,30},{-60,50}})));
  Modelica.Blocks.Routing.Multiplex3 multiplex3_1
    annotation (Placement(transformation(extent={{-40,0},{-20,20}})));
  ThermalZone zon(
    fmuName="bld.fmu",
    zoneName="Zone 1",
    redeclare package Medium = Medium,
    energyDynamics=Modelica.Fluid.Types.Dynamics.FixedInitial,
    nPorts=2)  "Thermal zone"
    annotation (Placement(transformation(extent={{20,-20},{60,20}})));
  Fluid.FixedResistances.PressureDrop duc(
    allowFlowReversal=false,
    linearized=true,
    from_dp=true,
    dp_nominal=100,
    m_flow_nominal=47*6/3600*1.2,
    redeclare package Medium = Medium)
    "Duct resistance (to decouple room and outside pressure)"
    annotation (Placement(transformation(extent={{10,-50},{-10,-30}})));
  Fluid.Sources.MassFlowSource_T bou(
    redeclare package Medium = Medium,
    nPorts=1,
    m_flow=0,
    T=293.15) "Boundary condition"
    annotation (Placement(transformation(extent={{-40,-90},{-20,-70}})));
  Fluid.Sources.Boundary_pT
                        freshAir(          redeclare package Medium = Medium,
      nPorts=1)
    "Boundary condition"
    annotation (Placement(transformation(extent={{-40,-50},{-20,-30}})));
  Modelica.Blocks.Sources.Constant qLatGai_flow(k=0) "Latent heat gain"
    annotation (Placement(transformation(extent={{-80,-30},{-60,-10}})));
equation
  connect(qRadGai_flow.y,multiplex3_1. u1[1])  annotation (Line(
      points={{-59,40},{-52,40},{-52,17},{-42,17}},
      color={0,0,127},
      smooth=Smooth.None));
  connect(qConGai_flow.y,multiplex3_1. u2[1]) annotation (Line(
      points={{-59,10},{-42,10}},
      color={0,0,127},
      smooth=Smooth.None));
  connect(zon.qGai_flow, multiplex3_1.y)
    annotation (Line(points={{18,10},{-19,10}}, color={0,0,127}));
  connect(multiplex3_1.u3[1], qLatGai_flow.y) annotation (Line(points={{-42,3},
          {-52,3},{-52,-20},{-59,-20}}, color={0,0,127}));
  connect(freshAir.ports[1], duc.port_b)
    annotation (Line(points={{-20,-40},{-10,-40}}, color={0,127,255}));
  connect(duc.port_a, zon.ports[1]) annotation (Line(points={{10,-40},{38,-40},
          {38,-19.2}}, color={0,127,255}));
  connect(bou.ports[1], zon.ports[2]) annotation (Line(points={{-20,-80},{42,
          -80},{42,-19.2}}, color={0,127,255}));
  annotation (Documentation(info="<html>
<p>
Simple test case for one buildings with one thermal zone.
</p>
</html>", revisions="<html>
<ul><li>
March 1, 2018, by Michael Wetter:<br/>
First implementation.
</li>
</ul>
</html>"),
 __Dymola_Commands(file="modelica://Buildings/Resources/Scripts/Dymola/ThermalZones/EnergyPlus/Validation/OneZone.mos"
        "Simulate and plot"),
experiment(
      StopTime=86400,
      Tolerance=1e-06));
end OneZone;
