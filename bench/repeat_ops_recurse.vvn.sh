cat <<EOF
fn recurseOps of x is
  if x != 0 do
    ret recurseOps(x - 1) + x - x * x / x;
  else
    ret 0;
  end
end

recurseOps(50000);
