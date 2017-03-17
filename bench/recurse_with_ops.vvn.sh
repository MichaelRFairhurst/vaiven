cat <<EOF
fn recurseOps of x is
  if x != 0 do
    ret recurseOps(x - 1) + x - x * x / x;
  end
  ret 0;
end

recurseOps(50000);
recurseOps(50000);
recurseOps(50000);
recurseOps(50000);
recurseOps(50000);
recurseOps(50000);
recurseOps(50000);
recurseOps(50000);
recurseOps(50000);
recurseOps(50000);
EOF
