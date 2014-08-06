within Buildings.Electrical.AC.OnePhase.Lines;
model TwoPortRL
  "Model of a resistive-inductive element with two electrical ports"
  extends Buildings.Electrical.Transmission.BaseClasses.PartialTwoPortRLC(
    redeclare package PhaseSystem_p = PhaseSystems.OnePhase,
    redeclare package PhaseSystem_n = PhaseSystems.OnePhase,
    redeclare Interfaces.Terminal_n terminal_n(redeclare package PhaseSystem =
          PhaseSystem_n),
    redeclare Interfaces.Terminal_p terminal_p(redeclare package PhaseSystem =
          PhaseSystem_p),
    final C=0);
  parameter Buildings.Electrical.Types.Assumption mode(
    min=Buildings.Electrical.Types.Assumption.FixedZ_steady_state,
    max=Buildings.Electrical.Types.Assumption.FixedZ_dynamic)=Buildings.Electrical.Types.Assumption.FixedZ_steady_state
    annotation(Evaluate=true,Dialog(group="Modelling assumption"));
protected
  Modelica.SIunits.AngularVelocity omega;
equation

  omega = der(PhaseSystem_p.thetaRef(terminal_p.theta));

  terminal_p.i = - terminal_n.i;

  if mode==Buildings.Electrical.Types.Assumption.FixedZ_dynamic then
    // Dynamic of the system
    der(L*terminal_p.i) + L*omega*PhaseSystem_p.j(terminal_p.i) + terminal_p.i*diagonal(ones(PhaseSystem_p.n)*R_actual) = terminal_p.v - terminal_n.v;

  else
    // steady state relationship
    L*omega*PhaseSystem_p.j(terminal_p.i) + terminal_p.i*diagonal(ones(PhaseSystem_p.n)*R_actual) = terminal_p.v - terminal_n.v;

  end if;

  annotation (Diagram(graphics={
          Rectangle(extent={{-70,30},{70,-30}}, lineColor={0,0,0}),
          Line(points={{-90,0},{-70,0}}, color={0,0,0}),
          Line(points={{70,0},{90,0}}, color={0,0,0})}),     Icon(
        coordinateSystem(preserveAspectRatio=false, extent={{-100,-100},{100,100}}),
                                                                  graphics={
          Text(
            extent={{-140,80},{140,40}},
            lineColor={0,120,120},
          textString="%name")}));
end TwoPortRL;